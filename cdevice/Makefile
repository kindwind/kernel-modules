obj-m := cdevice.o

#.PHONY: _user _module

all: _module _user

_user:
	gcc cdevice_user.c -o cdevice_user

_module:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules


clean_user:
	rm -rf cdevice_user

clean_module:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean

clean: clean_user clean_module
