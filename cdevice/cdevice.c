#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>	/* for dev_t */
#include <linux/kdev_t.h>	/* for MKDEV */
#include <linux/cdev.h>		/* for sturct cdev */
#include <linux/slab.h>		/* for kmalloc(), kzalloc() */
#include <asm/uaccess.h>	/* for copy_to_user(), copy_from_user() */
#include <linux/version.h>

#define MODULE_NAME "cdevice"
#define MAJOR_NUM 0
#define MINOR_NUM 0
#define DEVICE_NAME "cdevice"

unsigned int cdev_major = MAJOR_NUM, cdev_minor = MINOR_NUM;
struct cdev *cdevp = NULL; /* Char device structure */

static ssize_t cdevice_read(struct file *filp, char * __user buf, size_t count, loff_t *ppos)
{
	//char *msg = "cdevice read\n";
	char *msg = "cdevice read\n";
	ssize_t retval = 0;

	printk(KERN_INFO "cdevice read count = %d, offset = %lld\n",count, *ppos);
	if (*ppos >= strlen(msg))
		goto out;
	if (*ppos + count > strlen(msg) )
		count = strlen(msg) - *ppos;
	
	if (copy_to_user(buf, msg, count)) {
		retval = -EFAULT;
		goto out;
	}
	*ppos += count;
	retval = count;
out:
	return retval;
}

static ssize_t cdevice_write(struct file *filp, const char * __user buf, size_t count, loff_t *ppos)
{
	ssize_t retval = -ENOMEM; /* value used in "goto out" statements */
	char *datap = NULL;
	printk(KERN_INFO "cdevice write\n");
	datap = kzalloc(count, GFP_KERNEL);
    	if (datap == NULL) {
        	printk(KERN_INFO "kzalloc failed\n");
        	goto out;
	}
	if (copy_from_user(datap, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	printk(KERN_INFO "data = %s\n",datap);
	*ppos += count;
	retval = count;
	if( datap )
		kfree(datap);
	return count;
out:
	if(datap)
		kfree(datap);
	return retval;
}

// do any initialization in preparation for later operations
// struct file represents an open file, and struct inode represents a file on disk.
static int cdevice_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "cdevice open\n");
	return 0;
}

// Shut down the device on last close
static int cdevice_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "cdevice close\n");
	return 0;
}

struct file_operations cdevice_fops = {
	.owner = THIS_MODULE,
	.read = cdevice_read,
	.write = cdevice_write,
	.open = cdevice_open,
	.release = cdevice_release
};


static int __init init_cdevice(void)
{
	// dev_t is the type used to represent device numbers within the kernel
	dev_t devno;
	int result = 0, err = 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
	result = register_chrdev(cdev_major, DEVICE_NAME, &cdevice_fops);
#else
	if ( cdev_major ) {
		// Macro that builds a dev_t data item from the major and minor numbers
		devno = MKDEV(cdev_major, cdev_minor);
		// allow a driver to allocate ranges of device numbers.
		result = register_chrdev_region(devno, 1, DEVICE_NAME);
	} else {
		// allow a driver to allocate ranges of device numbers.
		// register_chrdev_region should be used when the desired major number is known
		result = alloc_chrdev_region(&devno, cdev_minor, 1, DEVICE_NAME);
		// Macros that extract the major and minor numbers from a device number.
		cdev_major = MAJOR(devno);
		// cdev_minor = MINOR(devno);
	}
#endif
	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", cdev_major);
		return result;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
#else
	cdevp = kzalloc(sizeof(struct cdev), GFP_KERNEL);
    	if (cdevp == NULL) {
        	printk(KERN_INFO "kzalloc failed\n");
        	goto fail;
    	}
	cdev_init(cdevp, &cdevice_fops);
	cdevp->owner = THIS_MODULE;
	err = cdev_add(cdevp, devno, 1);
	if (err){
		printk(KERN_NOTICE "Error %d adding cdevice", err);
		goto fail;
	}
	printk(KERN_INFO "cdevice init: major = %d, minor = %d\n",cdev_major,cdev_minor);
	return 0;
#endif
fail:
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
	// allow a driver to free ranges of device numbers.
	unregister_chrdev(cdev_major, DEVICE_NAME);
#else
	if (cdevp) {
        	kfree(cdevp);
        	cdevp = NULL;
    	}
    	unregister_chrdev_region(devno, 1); 
#endif
    	return err;  
}

static void __exit exit_cdevice(void)
{
	dev_t dev;

    	dev = MKDEV(cdev_major, cdev_minor);
	if( cdevp )
		cdev_del(cdevp);
	// allow a driver to free ranges of device numbers.
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
	unregister_chrdev(cdev_major, DEVICE_NAME);
#else
    	unregister_chrdev_region(dev, 1);
#endif
	printk(KERN_INFO "cdevice exit\n");
}


MODULE_DESCRIPTION("A character device driver");
MODULE_AUTHOR("Daniel Chen <kindwindster@gmail.com>");
MODULE_LICENSE("GPL");

module_init(init_cdevice);
module_exit(exit_cdevice);
