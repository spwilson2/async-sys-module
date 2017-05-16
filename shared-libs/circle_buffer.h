#ifndef __SHARED_LIBS_CIRCLE_BUFFER_H
#define __SHARED_LIBS_CIRCLE_BUFFER_H


#ifdef _LINUX_
#include <linux/mutex.h>
#else
#include <stddef.h>
#include <stdbool.h>
#include "mutex.h"
#endif

typedef struct __circle_buffer {
    size_t data_size;
    size_t size;

    struct mutex tail_lock;
    volatile size_t tail_idx; /* The start for the producer to place data. */
    volatile size_t head_idx; /* The beginning of a consumer list */

    char buffer[0]; /* The first address of the circular buffer. Variable length.*/
} circle_buffer;


int init_buffer(circle_buffer* buf, size_t data_size, size_t entries);

/* Insert the given `void *` into the circular_buffer, may block if no space.. */
void push(circle_buffer* buf, void* val_p);

/* 
 * Pop the given `void *` out of the circular_buffer if it there is anything to consume
 * will block if there is nothing to consume.
 */
void pop(circle_buffer* buf, void* val_p);

/* 
 * Will check the queue to see if there is anything remaining in the queue.
 */
extern inline int is_empty(circle_buffer *buf);

/* Check if there is enough space between head and tail to fit size */
extern inline int is_full(circle_buffer *buf);

#endif
