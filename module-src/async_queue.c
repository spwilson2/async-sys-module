#include <linux/fs.h>
#include <linux/stddef.h>
#include <linux/rwlock.h>

#include <as_sys/ioctl.h>

#include "buffer.h"
#include "async_queue.h"
#include "../shared-libs/circle-buffer.h"

#define QUEUE_SIZE(events) sizeof(circle_buffer) + sizeof(struct async_cb)*events

struct queue_metadata {
	/**
	 * @nr_events	Number of supported events in this queue. The user
	 * space head might lie or be corrupted so we keep redundant copy here.
	 */
	unsigned long nr_events;
	circle_buffer *syscall_queue;
};

int
init_async_queue(unsigned long nr_events, struct file *file, async_context_t *ctx_id)
{
	struct buffer_slab *buffer_slab;
	struct queue_metadata *queue_metadata;

	/* First try creating the buffer region for us to store the queue. */
	/* NOTE: We should be given the buffer_slab holding its lock. */
	if (!alloc_buffer(QUEUE_SIZE(nr_events), sizeof(struct queue_metadata),
				file, &buffer_slab))
		return false;

	/* Fill in the metadata head of the queue. */
	queue_metadata = buffer_slab->kernel_buffer;
	queue_metadata->nr_events = nr_events;
	queue_metadata->syscall_queue = buffer_slab->user_buffer;

	init_buffer((circle_buffer*)&queue_metadata->syscall_queue, sizeof(struct async_cb), nr_events);

	*ctx_id = buffer_slab->key.buffer_uid;
	/* We have set up our queue manager for the buffer_slab. We can release
	 * its lock.
	 */
	write_unlock(&buffer_slab->rwlock);
	return true;
}

void
deinit_async_queue(struct file *file, async_context_t *ctx_id)
{
	struct buffer_slab *buffer_slab;
	read_lock(&file->f_owner.lock);
	if (!get_buffer(*ctx_id, pid_nr(file->f_owner.pid), &buffer_slab))
		return;

	free_buffer_slab(buffer_slab);
	read_unlock(&file->f_owner.lock);
}
