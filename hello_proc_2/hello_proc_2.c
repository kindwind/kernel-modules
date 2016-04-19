/*
 * Linux Kernel 2.6.39
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>	/* for kmalloc() */
#include <asm/uaccess.h>	/* for copy_from_user */
#include <linux/seq_file.h>	/* for seq_read */
#include <linux/version.h>

MODULE_DESCRIPTION("Hello Proc Filesystem");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Chen <kindwindser@gmail.com");

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
int hello_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
#else
ssize_t hello_proc_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
#endif
{
	char temp[] = "This is hello proc read function";
        printk(KERN_INFO "hello proc read\n");
        if (copy_to_user(buf, temp, sizeof(temp)))
                return -EFAULT;

        return sizeof(temp);
}


#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
int hello_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
#else
ssize_t hello_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
#endif
{
	char *p = NULL;
	char arg0[256]={0}, arg1[256]={0}, arg2[256]={0};
	printk(KERN_DEBUG "hello proc write\n");
	if(!(p=kmalloc(count+1, GFP_KERNEL))) goto err;
	if(copy_from_user(p, buf, count)) goto err;
	sscanf(p, "%s %s %s", arg0, arg1, arg2);
	if( strcmp(arg0, "") ){
		printk(KERN_INFO "arg0 = %s\n",arg0);
	}
	if( strcmp(arg1, "") ){
		printk(KERN_INFO "arg1 = %s\n",arg1);
	}
	if( strcmp(arg2, "") ){
		printk(KERN_INFO "arg2 = %s\n",arg2);
	}
	return count;
	err:
        if(p) kfree(p);
        printk("%s, %d: err\n", __FUNCTION__, __LINE__);
        return -1;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
#else
struct file_operations hello_proc_fops =
{
        .owner = THIS_MODULE,
        .read = hello_proc_read,
        .write = hello_proc_write
};
#endif

int __init init_hello_proc(void)
{
	struct proc_dir_entry *hello = NULL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
	hello = create_proc_read_entry("hello_proc_2", 0, NULL, hello_proc_read, NULL);
#else
	hello = proc_create("hello_proc_2", 0, NULL, &hello_proc_fops);
#endif
	if(hello == NULL)
   		return -ENOMEM;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
	hello->write_proc = hello_proc_write;
#else
#endif
	printk(KERN_INFO "hello proc world!\n");
	return 0;
}

void __exit exit_hello_proc(void)
{
	remove_proc_entry("hello_proc_2", NULL);
	printk(KERN_INFO "Goodbye, hello_proc\n");
}

module_init(init_hello_proc);
module_exit(exit_hello_proc);
