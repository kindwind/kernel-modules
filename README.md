# kernel-modules

problems encountered:

1.
message: 
	make[2]: *** No rule to make target `arch/x86/entry/syscalls/syscall_32.tbl', needed by `arch/x86/entry/syscalls/../../include/generated/asm/syscalls_32.h'.  Stop.
	make[1]: *** [archheaders] Error 2

solution:
	sudo apt-get install linux-source


usage:
1.
kernel module with parameter array:
sudo insmod module-name parameter-array-name parameter-array-element-1,parameter-array-element-2,...,parameter-array-element-n


History:
20161118
    1.	Integraed codes in GitHub with Jenkins (Continuous Integration Server) to make it auto build.
