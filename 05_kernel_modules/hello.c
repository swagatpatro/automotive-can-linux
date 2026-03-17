/**
 * hello.c
 * Hello World Linux Kernel Module
 *
 * First kernel module — demonstrates:
 * -> kernel module structure
 * -> init and exit functions
 * -> printk kernel logging
 * -> module loading and unloading
 *
 * AUTOSAR Equivalent:
 * hello_init() = Can_Init()
 * hello_exit() = Can_DeInit()
 * printk()     = Det_ReportError()
 *
 * Commands:
 * sudo insmod hello.ko  -> loads module  -> calls hello_init()
 * sudo rmmod hello      -> removes module-> calls hello_exit()
 * sudo dmesg | tail -5  -> view kernel log
 */

#include <linux/module.h>   // module_init, module_exit, MODULE_LICENSE
#include <linux/kernel.h>   // printk, KERN_INFO

/**
 * hello_init - called when module is loaded
 *
 * Triggered by: sudo insmod hello.ko
 * Prints Hello World to kernel log
 * Must return 0 on success!
 * Non-zero return = module load fails!
 *
 * AUTOSAR Equivalent: Can_Init()
 */
static int __init hello_init(void)
{
    /*
     * printk = kernel equivalent of printf
     * KERN_INFO = informational message level
     * view output with: sudo dmesg | tail -5
     *
     * __init tells kernel:
     * -> this function only needed at startup
     * -> free this memory after init completes!
     */
    printk(KERN_INFO "Hello World\n");
    return 0;  // 0 = success, non-zero = failure
}

/**
 * hello_exit - called when module is removed
 *
 * Triggered by: sudo rmmod hello
 * Prints Goodbye to kernel log
 * Cleanup all resources here!
 *
 * AUTOSAR Equivalent: Can_DeInit()
 */
static void __exit hello_exit(void)
{
    /*
     * __exit tells kernel:
     * -> this function only needed at removal
     * -> optimizes kernel memory usage
     */
    printk(KERN_INFO "Goodbye!\n");
}

/*
 * Register init and exit functions with kernel
 * kernel calls hello_init() on insmod
 * kernel calls hello_exit() on rmmod
 */
module_init(hello_init);
module_exit(hello_exit);

/*
 * Module information
 * MODULE_LICENSE must be GPL for kernel access!
 * Non-GPL = restricted kernel API access!
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swagat");
MODULE_DESCRIPTION("Hello World Linux Kernel Module");
MODULE_VERSION("1.0");