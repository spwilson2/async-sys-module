/*
* Copyright (c) 2017 Sean Wilson <spwilson2.edu>
*
* This file is released under the GPLv2
*/
#ifndef __MODULE_SRC_COMMON_H
#define __MODULE_SRC_COMMON_H
#define mpr_warn(args...) pr_warn("as_sys: " args)
#define mpr_info(args...) pr_info("as_sys: " args)
#define mpr_err(args...)  pr_err("as_sys: " args)
#define trace() mpr_info("%s: %s - %d", __FILE__, __func__, __LINE__)

#include <linux/slab.h>
static __always_inline void *_dkmalloc(size_t size, gfp_t flags, const char*flag_string) {
	void *ret = kmalloc(size, flags);
	mpr_info("dkmalloc(%ld, %s) -> %p", size, flag_string, ret);
	return ret;
}
#define dkmalloc(SIZE, FLAGS) _dkmalloc(SIZE, FLAGS, #FLAGS)

static __always_inline void dkfree(void* ptr) {
	mpr_info("dkfree(%p)", ptr);
	kfree(ptr);
}
#endif
