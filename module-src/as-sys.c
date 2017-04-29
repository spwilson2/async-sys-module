
/*  
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>
#include <linux/miscdevice.h>


static int my_open(struct inode *i, struct file *f) {
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}
static int my_close(struct inode *i, struct file *f) {
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Driver: read()\n");
    return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Driver: write()\n");
    return len;
}

// Use our simple above defined ops to fill this function pointer interface out.
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

static struct miscdevice sample_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "as-sys",
    .fops = &fops,
    // NOTE: This could be configured with udev rules...
    .mode = S_ISVTX | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
};

int init_module(void)
{
    /* 
     * Create a special device so that people can use that device to
     * communicate with this module.  
     */

    int error;
    if (error = misc_register(&sample_device)) {
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

void cleanup_module(void)
{
    misc_deregister(&sample_device);
	printk(KERN_INFO "Async-sys closing\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sean Wilson <spwilson2@wisc.edu>");
MODULE_DESCRIPTION("Async Syscall Module");
