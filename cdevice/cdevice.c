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
#include <linux/proc_fs.h>	/* for proc file system */
#include <asm/atomic.h>		/* for race condition */

#include "cdevice.h"

#define MODULE_NAME "cdevice"
#define MAJOR_NUM 0
#define MINOR_NUM 0
#define DEVICE_NAME "cdevice"
#define PROC_NAME "cdevice"

unsigned int cdev_major = MAJOR_NUM, cdev_minor = MINOR_NUM;
struct cdev *cdevp = NULL; /* Char device structure */

atomic_t v = ATOMIC_INIT(0);
int gi = 1;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 0))
long cdevice_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg){
	int ret = -EINVAL;
	int err = 0;
	CDEV_CMD_SET_PARAM data_set;
	CDEV_CMD_GET_PARAM data_get;
	CDEV_CMD_EXCHANGE_PARAM data_exchange;
	CDEV_CMD_EXCHANGE_PARAM data;
	int value_get, value_set;
	printk("character device ioctl\n");

	data_get.val = 28;
	value_get = 299;

	data.val = 199;
	/*
	* extract the type and number bitfields, and don't decode
	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok( )
	*/
	if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > CDEV_IOC_MAXNR) return -ENOTTY;

	/*
	* the direction is a bitmask, and VERIFY_WRITE catches R/W
	* transfers. `Type' is user-oriented, while
	* access_ok is kernel-oriented, so the concept of "read" and
	* "write" is reversed
	*/
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	
	if (err) return -EFAULT;

	switch (cmd){
		case CDEV_IOC_CMD_INIT:
			printk("command init\n");
			atomic_set(&v, gi);
			printk("set atomic v: %d\n", atomic_read(&v));
			atomic_inc(&v);
			printk("increase atomic v: %d\n", atomic_read(&v));
			atomic_dec(&v);
			printk("decrease atomic v: %d\n", atomic_read(&v));
			err = atomic_inc_and_test(&v); // if atomic value is 0, then the return value is true; otherwise, it is false
			printk("increase and test atomic return: %d, v: %d\n", err, atomic_read(&v));
			err = atomic_dec_and_test(&v); // if atomic value is 0, then the return value is true; otherwise, it is false
			printk("decrease and test atomic return: %d, v: %d\n", err, atomic_read(&v));
			ret = 0;
			break;
		case CDEV_IOC_CMD_GET_VALUE:
			printk("command get value\n");
			
			return value_get;
			break;
		case CDEV_IOC_CMD_GET_POINTER:
			printk("command get pointer\n");
			
			if ( !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd)) ) {
				return -EFAULT;
			}
			if ( copy_to_user((int __user *)arg, &data_get, sizeof(data_get)) ) {
				return -EFAULT;
			}
			ret = 0;
			break;
		case CDEV_IOC_CMD_SET_VALUE:
			printk("command set value\n");
			
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			value_set = arg;
			printk("value_set: %d\n", value_set);
			printk("atomic v: %d\n", atomic_read(&v));
			atomic_add(value_set, &v);
			printk("add atomic v: %d\n", atomic_read(&v));
			atomic_sub(value_set, &v);
			printk("subtract atomic v: %d\n", atomic_read(&v));
			err = atomic_sub_and_test(value_set, &v); // if atomic value is 0, then the return value is true; otherwise, it is false
			printk("subtract and test atomic return: %d, v: %d\n", err, atomic_read(&v));
			ret = 0;
			break;
		case CDEV_IOC_CMD_SET_POINTER:
			printk("command set pointer\n");
			
			if (! capable (CAP_SYS_ADMIN)) {
				printk("not CAP_SYS_ADMIN\n");
				return -EPERM;
			}
			if ( copy_from_user(&data_set, (int __user *)arg, sizeof(data_set)) ) {
    			return -EFAULT;
			}
			printk("data_set.val: %d\n", data_set.val);
			printk("atomic v: %d\n", atomic_read(&v));
			err = atomic_add_negative(data_set.val, &v); // return value is true if the result is negative, false otherwise
			printk("add negative atomic return: %d, v: %d\n", err, atomic_read(&v));
			ret = 0;
			break;
		case CDEV_IOC_CMD_EXCHANGE:
			if (! capable (CAP_SYS_ADMIN))
				return -EPERM;
			data_exchange.val = data.val;
			if ( copy_from_user(&data, (int __user *)arg, sizeof(data)) ) {
    			return -EFAULT;
			}
			printk("data.val: %d\n", data.val);
			printk("atomic v: %d\n", atomic_read(&v));
			printk("add and return atomic v: %d\n", atomic_add_return(data.val, &v));
			printk("subtract and return atomic v: %d\n", atomic_sub_return(data.val, &v));
			printk("increase and return atomic v: %d\n", atomic_inc_return(&v));
			printk("decrease and return atomic v: %d\n", atomic_dec_return(&v));
			if ( copy_to_user((int __user *)arg, &data_exchange, sizeof(data_exchange)) ) {
				return -EFAULT;
			}
			ret = 0;
			break;
		default:
			printk("command Error\n");
			break;
	}
	return ret;
}
#else
int cdevice_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg){
	int error = -EINVAL;

	return error;
}
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
static ssize_t cdevice_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
	char msg[] = "hello character device proc read\n";
        //printk(KERN_INFO "hello proc read\n");
        if( copy_to_user(buf, msg, sizeof(msg)) ){
		return 0;
	}

	if(*ppos == 0)
	{
		*ppos += count;
		return count;
	}
	else
		return 0;
}
#else
static int cdevice_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data){
	int len = 0;
	len += sprintf(buffer+len, "hello character device proc read\n");
	*eof = 1;
	return len;
}
#endif

static ssize_t cdevice_read(struct file *filp, char * __user buf, size_t count, loff_t *ppos)
{
	//char *msg = "cdevice read\n";
	char *msg = "character device read\n";
	ssize_t retval = 0;

	printk(KERN_INFO "character device read count = %d, offset = %lld\n",count, *ppos);
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
	printk(KERN_INFO "character device write\n");
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
	printk(KERN_INFO "character device open\n");
	return 0;
}

// Shut down the device on last close
static int cdevice_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "character device close\n");
	return 0;
}

struct file_operations cdevice_fops = {
	.owner = THIS_MODULE,
	.read = cdevice_read,
	.write = cdevice_write,
	.open = cdevice_open,
	.release = cdevice_release,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 4, 0))
	.unlocked_ioctl = cdevice_unlocked_ioctl
#else
	.ioctl = cdevice_ioctl
#endif
};

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
struct file_operations cdevice_proc_fops = {
	.owner = THIS_MODULE,
	.read = cdevice_proc_read,
};
#endif

static int __init init_cdevice(void)
{
	dev_t devno;	// dev_t is the type used to represent device numbers within the kernel
	char buffer[128];
	int result = 0, err = 0;
	struct proc_dir_entry *ent;

	/* Register a character device */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
	result = register_chrdev(cdev_major, DEVICE_NAME, &cdevice_fops);	// DEVICE_NAME will appear in /proc/devices
#else
	if ( cdev_major ) { // specific major number
		// Macro that builds a dev_t data item from the major and minor numbers
		devno = MKDEV(cdev_major, cdev_minor);
		// allow a driver to allocate ranges of device numbers.
		// register_chrdev_region should be used when the desired major number is known
		result = register_chrdev_region(devno, 1, DEVICE_NAME);
	} else { // dynamic get major number, devno for output device number
		// allow a driver to allocate ranges of device numbers.
		result = alloc_chrdev_region(&devno, cdev_minor, 1, DEVICE_NAME);
		// Macros that extract the major and minor numbers from a device number.
		cdev_major = MAJOR(devno);
		// cdev_minor = MINOR(devno);
	}
#endif
	printk(KERN_INFO "===== %d =====\n",print_dev_t(buffer, devno)); //the number of characters in buffers
	printk(KERN_INFO "%s\n",buffer); //the number of characters in buffers
	memset(buffer, 0, 128);
	printk(KERN_INFO "===== %s =====\n",format_dev_t(buffer, devno));
	if ( result < 0 ) {
		printk(KERN_WARNING "can't get major %d\n", cdev_major);
		return result;
	}
	printk("major: %d\n", cdev_major);
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
	if (err) {
		printk(KERN_NOTICE "Error %d adding cdevice", err);
		goto fail;
	}
	printk(KERN_INFO "character device init: major = %d, minor = %d\n",cdev_major,cdev_minor);
#endif

	/* Create proc entry */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3, 9, 0))
	ent = proc_create(PROC_NAME, 0, NULL, &cdevice_proc_fops);
#else
	ent = create_proc_read_entry(PROC_NAME, 0, NULL, cdevice_proc_fops, NULL); // name, mode, parent proc entry, read proc function, data
#endif
	if ( !ent )	{
		printk(KERN_INFO "create proc read entry for character device fail\n");
	}

	return 0;
	
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
	
	remove_proc_entry(PROC_NAME, NULL);
	dev = MKDEV(cdev_major, cdev_minor);
	if ( cdevp )
		cdev_del(cdevp);
	// allow a driver to free ranges of device numbers.
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
	unregister_chrdev(cdev_major, DEVICE_NAME);
#else
	unregister_chrdev_region(dev, 1);
#endif
	printk(KERN_INFO "character device exit\n");
}


MODULE_DESCRIPTION("A character device driver");
MODULE_AUTHOR("Daniel Chen <kindwindster@gmail.com>");
MODULE_LICENSE("GPL");

module_init(init_cdevice);
module_exit(exit_cdevice);
