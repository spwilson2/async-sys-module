#ifndef __MODULE_SRC_IOCTL_H
#define __MODULE_SRC_IOCTL_H
#include <as_sys/ioctl.h>

int async_setup(unsigned long nr_events, async_context_t *ctx_idp);

int async_getevents(async_context_t ctx, long min_nr, long max_nr, 
                    struct async_event events[], struct timespec *timeout);

int async_destroy(async_context_t ctx);

#endif
