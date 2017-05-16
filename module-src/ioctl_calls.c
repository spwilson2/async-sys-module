#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/stddef.h>

#include <as_sys/ioctl.h>
#include "ioctl_calls.h"
#include "async_queue.h"

/**
 * async_setup() - Allocate a syscall buffer for the user
 * @nr_events:		The maximum number of system call events
 *			the buffer can suppoer
 * @ctx_idp		A pointer to the request context that will be updated
 *			to hold context information.
 *
 *
 */
int
async_setup(void *user_argument, struct file *file_p)
{
	struct _async_setup setup_args;
	async_context_t ctx_id;

	if (!access_ok(VERIFY_READ, user_argument, sizeof(setup_args)))
		return -1;
	if (copy_from_user(&setup_args, user_argument, sizeof(setup_args)))
		return -1;
	if (!access_ok(VERIFY_WRITE, setup_args.ctx_idp, sizeof(*setup_args.ctx_idp)))
		return -1;
	if (setup_args.nr_events > MAX_NR)
		return -1;

	if (!init_async_queue(setup_args.nr_events, file_p, &ctx_id))
		return -1;

	/* Copy out the async_context_t if it succeeded. */
	if (copy_to_user(setup_args.ctx_idp, &ctx_id, sizeof(ctx_id)))
		return -1;
	else
		/* Copying failed, let's clean up the state we just made. */
		deinit_async_queue(file_p, ctx_id);

	return 0;
}

/**
 * async_getevents() - Block until the number of events are serviced or timeout
 *			occurs
 * @ctx			The context number of the syscall queue
 * @min_nr		The minumum number of events to wait for
 * @max_nr		The maximum number of events to wait for
 */
int
async_getevents(void *user_argument, struct file *file_p)
{
	return -1; // TODO: UNSUPPORTED

	//struct _async_getevents getevents_args;
	//if (!access_ok(VERIFY_READ, user_argument, sizeof(getevents_args)))
	//	return -1;
	//if (copy_from_user(&getevents_args, user_argument, sizeof(getevents_args)))
	//	return -1;
}

/**
 * async_destroy() - Destroys the given context cleaning up all structures
 */
int
async_destroy(unsigned long user_argument, struct file *file_p)
{
	async_context_t ctx = (async_context_t)user_argument;
	deinit_async_queue(file_p, ctx);

	return 0;
}
