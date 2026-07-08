// SUBSYSTEM: XVK-INSTR | Component: sysfs_interface
// File: kernel/xvikil/sysfs/xvikil_sysfs.c
// Status: Implemented
// Phase: Instrumentation

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

static struct kobject *xvk_kobj;
static int tracing_enabled = 0; // Default: 0 (disabled)

static ssize_t tracing_enabled_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", tracing_enabled);
}

static ssize_t tracing_enabled_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret;
    ret = kstrtoint(buf, 10, &tracing_enabled);
    if (ret < 0)
        return ret;
    
    if (tracing_enabled) {
        printk(KERN_INFO "XVK-INSTR: Runtime tracing enabled via sysfs\n");
    } else {
        printk(KERN_INFO "XVK-INSTR: Runtime tracing disabled via sysfs\n");
    }
    return count;
}

static struct kobj_attribute tracing_enabled_attr = __ATTR(tracing_enabled, 0660, tracing_enabled_show, tracing_enabled_store);

static int __init xvikil_sysfs_init(void) {
    xvk_kobj = kobject_create_and_add("xvikil", kernel_kobj);
    if (!xvk_kobj)
        return -ENOMEM;
    
    if (sysfs_create_file(xvk_kobj, &tracing_enabled_attr.attr)) {
        printk(KERN_ERR "XVK-INSTR: Failed to create sysfs entry\n");
        kobject_put(xvk_kobj);
        return -ENOMEM;
    }
    
    printk(KERN_INFO "XVK-INSTR: /sys/kernel/xvikil/ initialized\n");
    return 0;
}

static void __exit xvikil_sysfs_exit(void) {
    sysfs_remove_file(xvk_kobj, &tracing_enabled_attr.attr);
    kobject_put(xvk_kobj);
}

module_init(xvikil_sysfs_init);
module_exit(xvikil_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Kernel Sysfs Interface");
