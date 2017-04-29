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

static inline void* compute_tail(void *tail, void *start, void *end, size_t size) {
        void *unwrapped_p = tail + size;
        void *newtail = (unwrapped_p < end) ? unwrapped_p : (void*)((size_t)start + unwrapped_p - end);
        return newtail;
}


/* Insert the given `void *` into the circular_buffer, may block if no space.. */
void push(circle_buffer* buf, void* val_p, size_t size) {

    //TODO: Need to insert the header as well....

    // Producers will grab a lock in order to solve queue ABA problems...
    //
    // (SEE: https://en.wikipedia.org/wiki/ABA_problem )
    //
    // The race condition that occurs in this ABA problem is that the whole
    // queue may loop around to this same location but the head might have
    // moved to now be in front of us and we would overwrite it. (It would
    // appear like the head to magically moved backwards, when in reality
    // we wrapped around the queue and thus ABA.)
    
    int placed = false;
    void* oldtail = (void*)buf->tail;
    void* newtail = compute_tail(oldtail, (void*)buf->start, (void*)buf->end, size);
    
    while (!placed) {
        // Spin until we can fit anything...
        while(!can_fit(buf, size))
            __asm_pause();

        // First precompute where we will place the tail after we are done with it.
        // Then grab the lock and assert it will still fit. (We do this to reduce
        // some lock contention.)
        void * tail = (void*)buf->tail;

        // We are now holding the lock, reconfirm we will fit.
        spin_lock(&buf->tail_lock);

        if (!can_fit(buf, size)) {
            spin_unlock(&buf->tail_lock);
            continue;
        }

        if (oldtail == buf->tail) // We can use our precomputed tail
            buf->tail = newtail;
        else // We need to recompute tail
            buf->tail = compute_tail((void*)buf->tail, (void*)buf->start, (void*)buf->end, size);
        placed = true;
        spin_unlock(&buf->tail_lock);
    }
}

/* 
 * Pop the given `void *` out of the circular_buffer if it there is anything to consume
 * will block if there is nothing to consume.
 */
void pop(circle_buffer* buf, void* val_p) {
}

/* 
 * Will check the queue to see if there is anything remaining in the queue.
 */
int is_empty(circle_buffer* buf) {
    assert(buf);
    return buf->head == buf->tail;
}

int can_fit(circle_buffer* buf, size_t size) {
    assert(buf);
    
    // Get cached versions of the datastructures so no race conditions occur.
    // TODO: Verify these operations are not reordered and are each atomic.
    void * head = (void*)buf->head;
    void * tail = (void*)buf->tail;
    
    // Check if we need to wrap around
    if ((tail + size) > buf->end) {

        // We would catch back up to the head to write this.
        if (head > tail) 
            return false;

        // Set size to the remaining amount at the from part of the circular buffer.
        size -= (buf->end - tail);
    }

    // Check if the space would put us past our final possible position.
    return (tail + size) > head - 1;
}
