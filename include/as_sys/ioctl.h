#ifndef __AS_SYS__IOCTL_H_
#define __AS_SYS__IOCTL_H_
/* Includes ioctl macros. */
#include <linux/ioctl.h>

/* Magic number for this ioctl driver */
#define AS_SYS_MAGIC 'a'


/* Create a shared memory ring with the kernel to be able to submit async requests */
#define AS_SYS_SETUP     _IORW(AS_SYS_MAGIC, 1, void*)
/* Block in the kernel for a set number of events or a timeout. */
#define AS_SYS_GETEVENTS _IO(AS_SYS_MAGIC,   2, void*)
/* Destroy the async ring manually */
#define AS_SYS_DESTROY   _IOW(AS_SYS_MAGIC,  3, unsigned int)

#endif
