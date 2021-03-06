#include <linux/ioctl.h>
#define CDEV_IOC_MAGIC 't'
#define NAME_SIZE 128

typedef enum CDEV_IOCTL_CMD_INDEX {
	CDEV_IOCTL_CMD_INIT_INDEX = 0,
	CDEV_IOCTL_CMD_GET_VALUE_INDEX,
	CDEV_IOCTL_CMD_GET_POINTER_INDEX,
	CDEV_IOCTL_CMD_SET_VALUE_INDEX,
	CDEV_IOCTL_CMD_SET_POINTER_INDEX,
	CDEV_IOCTL_CMD_EXCHANGE_INDEX,
	CDEV_IOCTL_CMD_MAX_INDEX
} CDEV_IOCTL_CMD_INDEX;

typedef enum CDEV_IOCTL_CMD_TYPE {
	CDEV_IOCTL_CMD_TYPE_INIT = 0,
	CDEV_IOCTL_CMD_TYPE_GET_VALUE,
	CDEV_IOCTL_CMD_TYPE_GET_POINTER,
	CDEV_IOCTL_CMD_TYPE_SET_VALUE,
	CDEV_IOCTL_CMD_TYPE_SET_POINTER,
	CDEV_IOCTL_CMD_TYPE_EXCHANGE,
	CDEV_IOCTL_CMD_TYPE_MAX
} CDEV_IOCTL_CMD_TYPE;

typedef struct {
	unsigned int reg;
	unsigned int val;
} CDEV_CMD_GET_PARAM;

typedef struct {
	unsigned int reg;
	unsigned int val;
} CDEV_CMD_SET_PARAM;

typedef struct {
	unsigned int reg;
	unsigned int val;
} CDEV_CMD_EXCHANGE_PARAM;

#define CDEV_IOC_CMD_INIT \
	_IO(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_INIT_INDEX)

#define CDEV_IOC_CMD_GET_VALUE \
	_IO(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_GET_VALUE_INDEX)

#define CDEV_IOC_CMD_GET_POINTER \
	_IOR(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_GET_POINTER_INDEX, CDEV_CMD_GET_PARAM)

#define CDEV_IOC_CMD_SET_VALUE \
	_IO(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_SET_VALUE_INDEX)

#define CDEV_IOC_CMD_SET_POINTER \
	_IOW(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_SET_POINTER_INDEX, CDEV_CMD_SET_PARAM)

#define CDEV_IOC_CMD_EXCHANGE \
	_IOW(CDEV_IOC_MAGIC, CDEV_IOCTL_CMD_EXCHANGE_INDEX, CDEV_CMD_EXCHANGE_PARAM)

#define CDEV_IOC_MAXNR CDEV_IOCTL_CMD_MAX_INDEX
