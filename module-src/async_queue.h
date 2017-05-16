#ifndef __Module_SRC_ASYNC_QUEUE_H
#define __Module_SRC_ASYNC_QUEUE_H

#include <linux/fs.h>
#include <as_sys/ioctl.h>
#include "buffer.h"

static inline int init_async_queue_file(struct file *file) 
{
	return buffer_init_file(file);
}
static inline void deinit_async_queue_file(struct file *file) 
{
	buffer_free_file(file);
}

/* Initilize the asynchronous queue with the given buffer and events size. */
int init_async_queue(unsigned long nr_events, struct file *file, async_context_t *ctx_id);
void deinit_async_queue(struct file *file, async_context_t ctx_id);


/* Try to get an event, if unable then fail out. */
int try_get_event(struct file *file, async_context_t ctx_id, struct async_event *async_event);

#endif
