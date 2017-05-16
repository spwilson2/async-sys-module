/*
* Copyright (c) 2017 Sean Wilson <spwilson2@wisc.edu>
*
* This file is released under the GPLv2
*/
#ifndef __MODULE_SRC_IOCTL_H
#define __MODULE_SRC_IOCTL_H
#include <as_sys/ioctl.h>

int async_setup(void *user_argument, struct file *file_p);

int async_getevents(void *user_argument, struct file *file_p);

int async_destroy(unsigned long, struct file *file_p);

#endif
