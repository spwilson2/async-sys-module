#ifndef __AS_SYS_IOCTL_H
#define __AS_SYS_IOCTL_H
/* Includes ioctl macros. */
#include <linux/types.h>
#include <asm/ioctl.h>

/* Magic number for this ioctl driver */
#define AS_SYS_MAGIC 'a'


/* Create a shared memory ring with the kernel to be able to submit async requests */
#define AS_SYS_SETUP     _IOWR(AS_SYS_MAGIC, 1, void*)
/* Block in the kernel for a set number of events or a timeout. */
#define AS_SYS_GETEVENTS _IOR(AS_SYS_MAGIC,   2, void*)
/* Destroy the async ring manually */
#define AS_SYS_DESTROY   _IOW(AS_SYS_MAGIC,  3, unsigned int)

struct async_cb {
	long number; /* The syscall number. */
	void * vargs[]; /* NULL terminated list of arguments to the syscall of given number. */
};

/* Used to store information about results. */
struct async_event {
	struct async_cb* cbp; /* Pointer to the async_cb where event came from. */
	__s64 res; /* Result of syscall. */
	// TODO: Might want other info like status code.
};

typedef __u64 async_context_t;

#endif
