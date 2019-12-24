#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_DESCRIPTION("a kernel module to get time");
MODULE_AUTHOR("Daniel Chen <kindwindster@gamil.com>");
MODULE_LICENSE("GPL");
#include <linux/rtc.h>
// #include <sys/time.h>
#include <linux/time.h>

#if 0
struct timespec current_kernel_time(void);
struct timeval {
	time_t      tv_sec;     /* seconds (long) */
	suseconds_t tv_usec;    /* microseconds (long) */
};

struct timezone {
	int tz_minuteswest;     /* minutes west of Greenwich */
	int tz_dsttime;         /* type of DST correction */
};

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;     /* unused */
	int tm_yday;     /* unused */
	int tm_isdst;    /* unused */
};

// Daylight Saving timeval
DST_NONE     /* not on DST */
DST_USA      /* USA style DST */
DST_AUST     /* Australian style DST */
DST_WET      /* Western European DST */
DST_MET      /* Middle European DST */
DST_EET      /* Eastern European DST */
DST_CAN      /* Canada */
DST_GB       /* Great Britain and Eire */
DST_RUM      /* Romania */
DST_TUR      /* Turkey */
DST_AUSTALT  /* Australian style with shift in 1986 */

#endif

static int __init init_get_time(void)
{
	struct timeval tv;
	do_gettimeofday(&tv);
	printk("sys_tz.tz_minuteswest = %d\n", sys_tz.tz_minuteswest);
	return 0;
}

static void __exit exit_get_time(void)
{
	printk(KERN_INFO "Goodbye, get time\n");	
}

module_init(init_get_time);
module_exit(exit_get_time);
