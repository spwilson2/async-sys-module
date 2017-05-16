#ifndef __Module_SRC_ASYNC_QUEUE_H
#define __Module_SRC_ASYNC_QUEUE_H

#include <linux/fs.h>
#include <as_sys/ioctl.h>

/* Initilize the asynchronous queue with the given buffer and events size. */
int init_async_queue(unsigned long nr_events, struct file *file, async_context_t *ctx_id);
void deinit_async_queue(struct file *file, async_context_t *ctx_id);

#endif
