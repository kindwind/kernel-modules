obj-m := cdevice.o

#.PHONY: _user _module

user_app := user_char_dev_op.c

default: all

all: _module _user

_user: $(user_app)
	gcc $(user_app) -o $(patsubst %.c, %, $<)

_module:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules


clean_user: $(user_app)
	rm -rf $(patsubst %.c, %, $<)
	rm -rf *.o

clean_module:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean

clean: clean_user clean_module
