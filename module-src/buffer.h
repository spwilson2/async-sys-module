#ifndef __MODULE_SRC_BUFFER_H
#define __MODULE_SRC_BUFFER_H

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/spinlock.h>

typedef unsigned long buffer_id_t;

struct buffer_slab {
	struct spinlock spinlock;
	void *buffer;
	buffer_id_t id;
};

/* Allocate a buffer for a given file. */
int alloc_buffer(size_t size, struct file *file, struct buffer_slab **buffer);

/* Free the buffer for the given id. */
void free_buffer(buffer_id_t id, struct file *file);

#endif
