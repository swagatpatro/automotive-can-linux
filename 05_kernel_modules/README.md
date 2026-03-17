# 05_kernel_modules - Linux Kernel Modules

## Overview
Linux kernel modules are pieces of code that can be
loaded and unloaded into the kernel on demand.
No reboot required!

## AUTOSAR Mapping
| Kernel Module | AUTOSAR Equivalent |
|---------------|--------------------|
| hello_init()  | Can_Init()         |
| hello_exit()  | Can_DeInit()       |
| printk()      | Det_ReportError()  |
| insmod        | ECU power on       |
| rmmod         | ECU power off      |
| dmesg         | DET log viewer     |
| .ko file      | .hex/.elf firmware |

## Key Concepts

### Kernel Space vs User Space
```
User Space (your programs so far):
-> protected memory
-> crash = only your program dies
-> uses printf, malloc
-> runs as normal user

Kernel Space (modules):
-> direct hardware access
-> crash = entire system crashes!
-> uses printk, kmalloc
-> runs with full privileges
```

### printk vs printf
```
printf  -> user space -> prints to terminal
printk  -> kernel space -> prints to kernel log
           view with: sudo dmesg | tail -5
```

### __init and __exit
```
__init -> function only needed at startup
       -> kernel frees memory after init!
       -> memory optimization!

__exit -> function only needed at removal
       -> kernel optimizes memory usage
```

### module_init and module_exit
```
module_init(hello_init)
-> registers init function with kernel
-> kernel calls it on insmod

module_exit(hello_exit)
-> registers exit function with kernel
-> kernel calls it on rmmod
```

## Programs

### hello.c
First kernel module — Hello World!
- Prints Hello World on load
- Prints Goodbye on unload
- Demonstrates basic module structure

## How to Build
```bash
make
```

## How to Run
```bash
# load module
sudo insmod hello.ko

# check kernel log
sudo dmesg | tail -5

# verify module loaded
lsmod | grep hello

# remove module
sudo rmmod hello

# check log again
sudo dmesg | tail -5
```

## Expected Output
```bash
# sudo dmesg | tail -5 after insmod:
[123456.789] Hello World

# sudo dmesg | tail -5 after rmmod:
[123456.789] Goodbye!
```

## Commands Reference
| Command | Meaning | What it does |
|---------|---------|--------------|
| make | build | compiles hello.c into hello.ko |
| sudo insmod hello.ko | insert module | loads into kernel, calls init() |
| sudo rmmod hello | remove module | unloads from kernel, calls exit() |
| sudo dmesg | tail -5 | kernel log | shows printk() output |
| lsmod | list modules | shows all loaded modules |
| cat -A Makefile | verify tabs | ^I = TAB correct |

## Files
| File | Purpose |
|------|---------|
| hello.c | kernel module source code |
| Makefile | build system |
| hello.ko | compiled kernel module (after make) |

## Requirements
- Linux Ubuntu 22/24
- kernel headers installed
- sudo privileges

## Install kernel headers
```bash
sudo apt install linux-headers-$(uname -r)
```

## Planned Modules
| Module | Description | Status |
|--------|-------------|--------|
| hello.c | Hello World | Done |
| char_driver.c | Character driver | Planned |
| vcan_driver.c | Virtual CAN driver | Planned |
| gpio_driver.c | GPIO driver | Planned |