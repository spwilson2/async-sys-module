#ifndef __CIRCLE_BUFFER_H__
#define __CIRCLE_BUFFER_H__

// Can't include stdlib.h
#include <stddef.h>
#include <stdbool.h>
#include "mutex.h"

typedef struct __circle_buffer {
    struct mutex tail_lock;
    volatile void * tail; /* The start for the producer to place data. */
    volatile void * head; /* The beginning of a consumer list */

    volatile void * end; /* The final address+1 of the circular buffer. */
    volatile void * start; /* The first address of the circular buffer. */
} circle_buffer;

/* Insert the given `void *` into the circular_buffer, may block if no space.. */
void push(circle_buffer* buf, void* val_p, size_t size);

/* 
 * Pop the given `void *` out of the circular_buffer if it there is anything to consume
 * will block if there is nothing to consume.
 */
void pop(circle_buffer* buf, void* val_p);

/* 
 * Will check the queue to see if there is anything remaining in the queue.
 */
int is_empty(circle_buffer* buf);

/* Check if there is enough space between head and tail to fit size */
int can_fit(circle_buffer* buf, size_t size);

#endif
