#include <linux/fs.h>
#include "ioctl_calls.h"

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

	unsigned long nr_events; 
	async_context_t *ctx_idp;
	/*
	 * 1. Check user_pointer is valid
	 * 1. Ensure that the number of requested events is possible for us to
	 * handle (less than MAX_NR).
	 * 1. Check that the ctx_idp is valid for userspace of this program.
	 * 1. Allocate a buffer for this context of size for nr_events.
	 * `alloc_buffer()`
	 * 1. Initilize the ctx_idp with the returned buffer id.
	 * `init_async_queue()`
	 */

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
	async_context_t ctx; 
	long min_nr; /* If 0, we won't block, just operates as a check. */
	long max_nr; /* If 0, will wait until timeout or all events are
			handled */
	struct async_event **events;
	struct timespec *timeout;

	/*
	 * 1. Check user_pointer is valid.
	 * 1. Assert that the ctx number is a valid number for this file.
	 * NOTE: Validate that the event pointers are valid before any event is
	 * returned.
	 * 1. 
	 */
}

/**
 * async_destroy() - Destroys the given context cleaning up all structures
 */
int 
async_destroy(void *user_argument, struct file *file_p)
{
	async_context_t ctx = (async_context_t)user_argument;
	/*
	 * 1. Validate the ctx number for this file.
	 * 1. Delete any datastructures in aysnc_queue (might not need to do).
	 * 1. Free the shared kernel-user memory ring buffer `free_buffer()`
	 */
}
