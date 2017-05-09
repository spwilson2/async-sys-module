
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/pid.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

#include <asm/uaccess.h>

#include <as_sys/ioctl.h>

/* Wrap the given callback syscall with memory space fixings so syscall check for correct
 * address space passes.
 */
static void wrap_syscall(void(*callback)(void)) {

	/* src: http://www.linux-mag.com/id/651/ */

	/* Save current fs and set it to valid address. */
	mm_segment_t fs = get_fs();
	set_fs(get_ds());

	/* system calls can be invoked */
	callback();

	/* Restore to return to user space */
	set_fs(fs);
}


static int my_open(struct inode *i, struct file *f) {
	unsigned long flags;

    printk(KERN_INFO "Driver: open()\n");
	// Assert we've received a file pointer from the kernel.
	if (!f) {
		printk(KERN_ERR "File pointer not given\n");
		return -1;
	}

	// Save the current task as the owner of the file.
	write_lock_irqsave(&f->f_owner.lock, flags);
	if (f->f_owner.pid) {
		printk(KERN_ERR "Pointer to file owners pid struct is already set\n");
		write_unlock_irqrestore(&f->f_owner.lock, flags);
		return -1;
	}
	f->f_owner.pid = get_task_pid(current, PIDTYPE_PID);
	write_unlock_irqrestore(&f->f_owner.lock, flags);

	printk(KERN_INFO "\t\t open pid = %d\n", current->pid);

    return 0;
}
static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg) {
	unsigned long flags;

	read_lock_irqsave(&f->f_owner.lock, flags);
	if (!f->f_owner.pid) {
		printk(KERN_ERR "Driver: ioctl called on file without pid owner set!!\n");
		read_unlock_irqrestore(&f->f_owner.lock, flags);
		return -1;
	}
    printk(KERN_INFO "Driver: ioctl(%u, %lu)\n", cmd, arg);
    printk(KERN_INFO "\t\t ioctl pid: %d\n", pid_nr(f->f_owner.pid));
	read_unlock_irqrestore(&f->f_owner.lock, flags);

    return 0;
}

// Use our simple above defined ops to fill this function pointer interface out.
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl,
};

static struct miscdevice sample_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "as_sys",
    .fops = &fops,
    // NOTE: This could be configured with udev rules...
    .mode = S_ISVTX | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
};

static int __init as_sys_init_module(void)
{
    /*
     * Create a special device so that people can use that device to
     * communicate with this module.
     */

    int error;
    if ((error = misc_register(&sample_device))) {
        pr_err("can't misc_register :(\n");
        return error;
    }

    sample_device.mode = S_IROTH | S_IWOTH;

	printk(KERN_INFO "Async-sys initilized\n");

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

static void __exit as_sys_cleanup_module(void)
{
    misc_deregister(&sample_device);
	printk(KERN_INFO "Async-sys closing\n");
}

module_init(as_sys_init_module);
module_exit(as_sys_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sean Wilson <spwilson2@wisc.edu>");
MODULE_DESCRIPTION("Async Syscall Module");
