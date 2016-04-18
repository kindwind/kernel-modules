# kernel-modules

problems encountered:

1.
message: 
	make[2]: *** No rule to make target `arch/x86/entry/syscalls/syscall_32.tbl', needed by `arch/x86/entry/syscalls/../../include/generated/asm/syscalls_32.h'.  Stop.
	make[1]: *** [archheaders] Error 2

solution:
	sudo apt-get install linux-source
