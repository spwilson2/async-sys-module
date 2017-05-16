/*
* Copyright (c) 2017 Sean Wilson <spwilson2@wisc.edu>
*
* This file is released under the GPLv2
*/

#include <linux/init.h>
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/pid.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/device.h>

#include <asm/uaccess.h>

#include <as_sys/ioctl.h>
#include "async_queue.h"
#include "ioctl_calls.h"
#include "common.h"

static void *sys_call_table;
static struct miscdevice sample_device;
/* Wrap the given callback syscall with memory space fixings so syscall check for correct
 * address space passes.
 */
// Doesn't seem necessary for syscalls that are in the kernel already.
//static void wrap_syscall(void(*callback)(void)) {
//
//	/* src: http://www.linux-mag.com/id/651/ */
//
//	/* Save current fs and set it to valid address. */
//	mm_segment_t fs = get_fs();
//	set_fs(get_ds());
//
//	/* system calls can be invoked */
//	callback();
//
//	/* Restore to return to user space */
//	set_fs(fs);
//}

static int
my_open(struct inode *i, struct file *f)
{
	mpr_info("Driver: open()\n");
	//// Assert we've received a file pointer from the kernel.
	if (!f) {
		mpr_err("File pointer not given\n");
		return -1;
	}

	// Save the current task as the owner of the file.
	write_lock(&f->f_owner.lock);
	if (f->f_owner.pid) {
		mpr_err("Pointer to file owners pid struct is already set\n");
		write_unlock(&f->f_owner.lock);
		return -1;
	}
	f->f_owner.pid = get_task_pid(current, PIDTYPE_PID);
	write_unlock(&f->f_owner.lock);

	mpr_info("\t\t open pid = %d\n", current->pid);

	/* This is actual code I want to keep now..*/
	if (f->private_data) {
		mpr_info("Why is private_data init on new file?\n");
		mpr_info("\t\t %p\n", f->private_data);
		f->private_data = 0;
	}

	if (!init_async_queue_file(f))
		return -1;
	return 0;
}

static int
my_close(struct inode *i, struct file *f) {
	mpr_info("Driver: close()\n");
	deinit_async_queue_file(f);
	return 0;
}

static long
my_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
	/*
	   unsigned long flags;

	   read_lock_irqsave(&f->f_owner.lock, flags);
	   if (!f->f_owner.pid) {
	   mpr_info("Driver: ioctl called on file without pid owner set!!\n");
	   read_unlock_irqrestore(&f->f_owner.lock, flags);
	   return -1;
	   }
	   mpr_info("Driver: ioctl(%u, %lu)\n", cmd, arg);
	   mpr_info("\t\t ioctl pid: %d\n", pid_nr(f->f_owner.pid));
	   read_unlock_irqrestore(&f->f_owner.lock, flags);

	// Call the argd'th syscall
	mpr_info("sys_call_table: %p\n", sys_call_table);
	void (**sys_call_addr)(void);
	sys_call_addr = sys_call_table + arg * sizeof(void*);
	mpr_info("sys_call_table entry address: %p\n", sys_call_addr);
	mpr_info("sys_call address: %p\n", *sys_call_addr);

	// Doesn't seem that the address space change is necessary.
	//wrap_syscall(*sys_call_addr);
	(*sys_call_addr)();
	*/

	// Ensure the magic header is intact.
	if (_IOC_TYPE(cmd) != AS_SYS_MAGIC) {
		mpr_info("Invalid Magic Header provided.\n");
		return -1;
	}

	// Switch into one of our supported functions.
	switch (cmd) {
		case AS_SYS_SETUP:
			return async_setup((void*)arg, f);
			break;
		case AS_SYS_GETEVENTS:
			return async_getevents((void*)arg, f);
			break;
		case AS_SYS_DESTROY:
			return async_destroy(arg, f);
			break;
		default:
			mpr_info("Invalid ioctl command.\n");
			mpr_info("\t\t cmd: 0x%x\n", cmd);
			mpr_info("\t\t arg: 0x%lx\n", arg);
			return -1;
	}
}

// Use our simple above defined ops to fill this function pointer interface out.
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.unlocked_ioctl = my_ioctl,
};

static struct miscdevice sample_device= {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "as_sys",
	.fops = &fops,
	// NOTE: This could/should be configured with udev rules...
	.mode = S_ISVTX | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
};

static int __init as_sys_init_module(void)
{
	int error;
	/*
	 * Create a special device so that people can use that device to
	 * communicate with this module.
	 */

	if ((error = misc_register(&sample_device))) {
		mpr_err("can't misc_register :(\n");
		return error;
	}

	sys_call_table = (void*) kallsyms_lookup_name("sys_call_table");
	mpr_info("sys_call_table addr: %p\n", sys_call_table);

	sample_device.mode = S_IROTH | S_IWOTH;

	mpr_info("Async-sys initilized\n");

	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}

static void __exit as_sys_cleanup_module(void)
{
	misc_deregister(&sample_device);
	mpr_info("Async-sys closing\n");
}

module_init(as_sys_init_module);
module_exit(as_sys_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sean Wilson <spwilson2@wisc.edu>");
MODULE_DESCRIPTION("Async Syscall Module");
