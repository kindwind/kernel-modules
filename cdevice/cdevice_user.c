#include <stdio.h>
#include <errno.h>		/* errno */
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit, getpid */
#include <string.h>

#include <sys/ioctl.h>
#include "cdevice.h"

#define NOT_INITIALIZED (-1)
#define DEVICE_NAME "/dev/cdevice0"

static int cdeviceFile = NOT_INITIALIZED;

void usage(){
	printf("usage: ./cdevice_user cmd<0,5>\n");
}

int main(int argc, char **argv){
	int cmd;
	int ret;
	int value_set;
	int value_get;
	CDEV_CMD_GET_PARAM param_get;
	CDEV_CMD_SET_PARAM param_set;
	CDEV_CMD_EXCHANGE_PARAM param_exchange;
	
	param_set.val = 999;
	value_set = 99;
	param_exchange.val = 888;

	if (argc==1){
		usage();
		return 0;
	}

	printf("argument: %s\n", argv[1]);

	cmd = atoi(argv[1]);
	if ( cmd < 0 || cmd >5) {
		usage();
		return 0;
	}

	printf("command: %d\n", cmd);

	if ( (cdeviceFile = open(DEVICE_NAME, O_RDWR)) == -1 ){
		printf("%s: cdevice open %s error %d\n", __FUNCTION__, DEVICE_NAME, errno);
		return -errno;
	}

	//printf("cdeviceFile: %d\n", cdeviceFile);
	
	switch (cmd) {
		case CDEV_IOCTL_CMD_TYPE_INIT:
			if (ioctl(cdeviceFile, CDEV_IOC_CMD_INIT) != 0){
				printf("%s: cdevice ioctl command %d error\n", __FUNCTION__, cmd);
				return -errno;
			}
			break;
		case CDEV_IOCTL_CMD_TYPE_GET_VALUE:
			value_get = ioctl(cdeviceFile, CDEV_IOC_CMD_GET_VALUE);
			printf("value_get: %d\n", value_get);
			break;
		case CDEV_IOCTL_CMD_TYPE_GET_POINTER:
			if (ioctl(cdeviceFile, CDEV_IOC_CMD_GET_POINTER, &param_get) != 0){
				printf("%s: cdevice ioctl command %d error %d\n", __FUNCTION__, cmd, errno);
				perror("IOCTL");
				return -errno;
			}
			printf("param_get.val: %d\n", param_get.val);
			break;
		case CDEV_IOCTL_CMD_TYPE_SET_VALUE:
			if ( ioctl(cdeviceFile, CDEV_IOC_CMD_SET_VALUE, value_set) != 0 ){
				printf("%s: cdevice ioctl command %d error %d\n", __FUNCTION__, cmd, errno);
				return -errno;
			}
			break;
		case CDEV_IOCTL_CMD_TYPE_SET_POINTER:
			if ( ioctl(cdeviceFile, CDEV_IOC_CMD_SET_POINTER, &param_set) != 0 ){
				printf("%s: cdevice ioctl command %d error %d, ret: %d\n", __FUNCTION__, cmd, errno, ret);
				perror("IOCTL");
				return -errno;
			}
			break;
		case CDEV_IOCTL_CMD_TYPE_EXCHANGE:
			printf("param_exchange.val: %d\n", param_exchange.val);
			if (ioctl(cdeviceFile, CDEV_IOC_CMD_EXCHANGE, &param_exchange) != 0){
				printf("%s: cdevice ioctl command %d error\n", __FUNCTION__, cmd);
				return -errno;
			}
			printf("param_exchange.val: %d\n", param_exchange.val);
			break;
		default:
			printf("%s: invalid cdevice ioctl command %d\n", __FUNCTION__, cmd);
			return 0;
	}
	
	if ( close(cdeviceFile) == -1 ){
		printf("%s: cdevice close error %d\n", __FUNCTION__, errno);
		return -errno;
	}

	cdeviceFile = NOT_INITIALIZED;
	return 0;
}
