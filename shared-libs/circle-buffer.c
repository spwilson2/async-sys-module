#include <string.h>
#include <assert.h>

#include "asm-primitives.h"
#include "mutex.h"
#include "circle-buffer.h"

/* 
 * A header containing metadata each entry into the buffer (so we can have
 * variable sized entries
 */
struct buffer_entry_header {
    size_t size;
};

static inline size_t increment_idx(size_t idx, size_t size) {
    idx += 1;
    return  idx >= size ? idx : 0;
}

void init_buffer(circle_buffer* buf, size_t data_size, size_t enteries) {
    assert(buf);
    buf->data_size = data_size;
    buf->size = enteries;
    buf->head_idx = buf->tail_idx = 0;
    buf->start = malloc(data_size * enteries);
}

void deinit_buffer(circle_buffer *buf) {
    assert(buf);
    free(buf->start);
    buf->start = NULL;
}

/* Insert the given `void *` into the circular_buffer, may block if no space.. */
void push(circle_buffer* buf, void* data_p) {

    ///////
    // Producers will grab a lock in order to solve queue ABA problems...
    //
    // (SEE: https://en.wikipedia.org/wiki/ABA_problem )
    //
    // The race condition that occurs in this ABA problem is that the whole
    // queue may loop around to this same location but the head might have
    // moved to now be in front of us and we would overwrite it. (It would
    // appear like the head to magically moved backwards, when in reality
    // we wrapped around the queue and thus ABA.)
    ///////
    for(;;) {
        // Spin until we can fit anything...
        while(!is_full(buf))
            __asm_pause();

        // We are now holding the lock, reconfirm we will fit.
        spin_lock(&buf->tail_lock);

        if (is_full(buf)) {
            spin_unlock(&buf->tail_lock);
            continue;
        }

        size_t oldtail = (size_t)buf->tail_idx;
        size_t newtail = increment_idx(oldtail, buf->size);

        // Place the data. (We have to do this with the lock to avoid race
        // conditions with the head. To reduce contention we could use a shadow
        // tail as well.)
        memcpy(buf->start + buf->data_size * oldtail, data_p, buf->data_size);

        buf->tail_idx = newtail;
        spin_unlock(&buf->tail_lock);
        break;
    }
}

/* 
 * Pop the given `void *` out of the circular_buffer and place it into the given dest
 * if there is anything to consume - will block if there is nothing to consume.
 */
void pop(circle_buffer* buf, void* dest_p) {

    /////
    // The readers can remain lock free since there is only a single pivot
    // point for them. (The head pointer)
    /////

    for(;;) {
        // Spin until we can fit anything...
        while(!is_empty(buf))
            __asm_pause();

        size_t oldhead = (size_t)buf->head_idx;
        size_t newhead = increment_idx(oldhead, buf->size);

        // Place the data into dest. 
        memcpy(dest_p, buf->start + buf->data_size * oldhead, buf->data_size);

        if (__sync_bool_compare_and_swap(&buf->head_idx, oldhead, newhead))
            break;
    }
}

/* 
 * Will check the queue to see if there is anything remaining in the queue.
 */
inline int is_empty(circle_buffer* buf) {
    assert(buf);
    return buf->head_idx == buf->tail_idx;
}

/*
 *
 */
inline int is_full(circle_buffer* buf) {
    assert(buf);
    return increment_idx(buf->head_idx, buf->size) == buf->tail_idx;
}
