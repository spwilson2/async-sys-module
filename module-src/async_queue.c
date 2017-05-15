#include <linux/fs.h>
#include <linux/stddef.h>
#include <linux/spinlock.h>

#include <as_sys/ioctl.h>

#include "buffer.h"
#include "async_queue.h"

#define QUEUE_SIZE(events) sizeof()

int
init_async_queue(unsigned long nr_events, struct file *file, async_context_t *ctx_id)
{
	struct buffer_slab *buffer_slab;

	/* First try creating the buffer region for us to store the queue. */
	/* NOTE: We should be given the buffer_slab holding its lock. */
	if (!alloc_buffer(QUEUE_SIZE(nr_events), file, &buffer_slab))
		return false;

	/* Fill in the */

	/* TODO: Once we have set up our queue manager for the buffer_slab we can
	 * release its lock.
	 */
	spin_unlock(&buffer_slab->spinlock);
	return false;
}
