#!/bin/sh

module="cdevice"
device="cdevice0"
mode="644"

user_app="cdevice_user"


option="${1}"
value="${2}"
case ${option} in 
	load)
		# invoke insmod with all arguments we got
		# and use a pathname, as newer modutils don't look in . by default
		/sbin/insmod `pwd`/$module.ko || exit 1

		# remove stale nodes
		rm -f /dev/${device}

		major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

		echo major number $major

		mknod /dev/${device} c $major 0

		# give appropriate group/permissions, and change the group.
		# Not all distributions have staff, some have "wheel" instead.
		group="staff"
		grep -q '^staff:' /etc/group || group="wheel"

		#chgrp $group /dev/${device}
		#chmod $mode /dev/${device}
		;; 
	unload)
		# echo $module
		# invoke rmmod with all arguments we got
		/sbin/rmmod $module || exit 1

		# Remove stale nodes

		rm -f /dev/${device}
		;; 
	run)
		./${user_app} ${value}
		;;
	*)
		echo "usage: ./`basename ${0}` operation <load|unload|run> [value <0,4>]"
      	exit 1 # Command to come out of the program with status 1
	;; 
esac 
