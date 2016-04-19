#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_DESCRIPTION("a kernel module with parameters");
MODULE_AUTHOR("Daniel Chen <kindwindster@gamil.com>");
MODULE_LICENSE("GPL");

static int howMany = 10;
static char *whom = "world";
static char *where[] = {"America", "Taiwan", "Japan"};

module_param(howMany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);
module_param_array(where, charp, 0, S_IRUGO);

static int __init init_hello_param(void)
{
	int i=0;
	printk(KERN_INFO "hello parameters: howMany = %d\n",howMany);
	printk(KERN_INFO "hello parameters: whom = %s\n",whom);
	for(;i<ARRAY_SIZE(where);i++){
		printk(KERN_INFO "hello parameters: where = %s\n",where[i]);
	}
	return 0;
}

static void __exit exit_hello_param(void)
{
	printk(KERN_INFO "Goodbye, hell parameters\n");	
}

module_init(init_hello_param);
module_exit(exit_hello_param);
