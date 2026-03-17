# Makefile
# Build system for Linux Kernel Modules
#
# Usage:
# make       -> builds kernel module (.ko file)
# make clean -> removes all build artifacts
#
# Output:
# hello.ko   -> loadable kernel module
#
# Commands after build:
# sudo insmod hello.ko  -> load module
# sudo rmmod hello      -> remove module
# sudo dmesg | tail -5  -> view kernel log

# obj-m = object file to build as module
# += means add to existing list
# hello.o -> kernel build system creates hello.ko
obj-m += hello.o

# all target -> default target when make is run
# -C /lib/modules/$(shell uname -r)/build
#    -> use current kernel build system
#    -> uname -r = current kernel version
#       example: 6.17.0-14-generic
# M=$(PWD)
#    -> our module source is in current directory
# modules
#    -> build target = compile kernel modules
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean target -> removes all build artifacts
# removes: .ko .o .mod .mod.c Module.symvers modules.order
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

## Important Reminder
#```
#Lines after all: and clean:
#MUST use TAB not spaces!
#
#If you copy paste this:
#-> verify TAB is there
#-> cat -A Makefile
#-> should show ^I before make