#ifndef __MODULE_SRC_IOCTL_H
#define __MODULE_SRC_IOCTL_H
#include <as_sys/ioctl.h>

int async_setup(void *user_argument, struct file *file_p);

int async_getevents(void *user_argument, struct file *file_p);

int async_destroy(void *user_argument, struct file *file_p);

#endif
