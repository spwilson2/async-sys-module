#include <string.h>
#include <assert.h>

#include "mutex.h"
#include "circle-buffer.h"

/* 
 * A header containing metadata each entry into the buffer (so we can have
 * variable sized entries
 */
struct buffer_entry_header {
    size_t size;
};


/* Insert the given `void *` into the circular_buffer, may block if no space.. */
void push(circle_buffer* buf, void* val_p, size_t size) {
    // 
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
