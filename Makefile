LKMODULE_DIRS = cdevice hello_param hello_proc hello_proc_2

default: all

all:
	for i in $(LKMODULE_DIRS); do \
		(cd $$i && make); \
	done

clean:
	for i in $(LKMODULE_DIRS); do \
		(cd $$i && make clean); \
	done
