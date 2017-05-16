/*
* Copyright (c) 2017 Sean Wilson <spwilson2.edu>
*
* This file is released under the GPLv2
*/
#ifndef __MODULE_SRC_BUFFER_H
#define __MODULE_SRC_BUFFER_H

#include <linux/types.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/pid.h>

typedef unsigned long buffer_id_t;


struct map_key {
	/* Both make up the key, <pid, buffer_uid> -> buffer. */
	pid_t pid;
	buffer_id_t buffer_uid;
};

struct buffer_slab {
	/** 
	 * This isn't a traditional rwlock, write is needed to delete this
	 * buffer slab but read is used to be able read/modify the internals 
	 * of the user_buffer and kernel_buffer.
	 *
	 * We solve the problem which could occur with a dynamically allocated
	 * lock (We lock a freed lock thinking it's still valid) by using
	 * hand-over-hand locking. The map_wrapper will act as a short time BFL
	 * to block other readers from being able to get to this on deletion.
	 */
	rwlock_t rwlock;
	void *user_buffer;
	void *kernel_buffer;
	struct map_key key;
};

/* Initilize the file's private_data for use in this module. */
int buffer_init_file(struct file *file);

/* Free all of the given file's buffers. */
void buffer_free_file(struct file *file);

/* Allocate a buffer for a given file. */
int alloc_buffer(size_t user_buffer_size, size_t kernel_buffer_size, 
		struct file *file, struct buffer_slab **buffer);

/* Get the buffer from the map. */
int get_buffer(buffer_id_t id, pid_t pid, struct buffer_slab **buffer);

/* Free the buffer for the given id. */
void free_buffer(buffer_id_t id, struct file *file);


#endif
