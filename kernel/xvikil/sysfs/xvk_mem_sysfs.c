// SUBSYSTEM: XVK-MEM | Component: mem_sysfs_interface
// File: kernel/xvikil/sysfs/xvk_mem_sysfs.c
// Status: Implemented
// Phase: Memory

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/mm.h>
#include <linux/string.h>

extern int sysctl_watermark_scale_factor; // From mm/page_alloc.c

static struct kobject *xvk_mem_kobj;
static int watermark_scale = 150; // Default: 150 (1.5x of free pages)

static ssize_t watermark_scale_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", sysctl_watermark_scale_factor);
}

static ssize_t watermark_scale_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;
    
    if (val < 10 || val > 1000)
        return -EINVAL;
        
    sysctl_watermark_scale_factor = val;
    printk(KERN_INFO "XVK-MEM: watermark_scale_factor set to %d\n", val);
    
    return count;
}

static struct kobj_attribute watermark_scale_attr = __ATTR(watermark_scale, 0660, watermark_scale_show, watermark_scale_store);

static int __init xvk_mem_sysfs_init(void) {
    xvk_mem_kobj = kobject_create_and_add("xvk_mem", kernel_kobj);
    if (!xvk_mem_kobj)
        return -ENOMEM;
    
    if (sysfs_create_file(xvk_mem_kobj, &watermark_scale_attr.attr)) {
        printk(KERN_ERR "XVK-MEM: Failed to create sysfs entry\n");
        kobject_put(xvk_mem_kobj);
        return -ENOMEM;
    }
    
    sysctl_watermark_scale_factor = watermark_scale;
    
    printk(KERN_INFO "XVK-MEM: Memory sysfs interface initialized\n");
    return 0;
}

static void __exit xvk_mem_sysfs_exit(void) {
    sysfs_remove_file(xvk_mem_kobj, &watermark_scale_attr.attr);
    kobject_put(xvk_mem_kobj);
}

module_init(xvk_mem_sysfs_init);
module_exit(xvk_mem_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Memory Sysfs Interface");
