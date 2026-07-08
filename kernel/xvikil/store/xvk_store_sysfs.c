// SUBSYSTEM: XVK-STORE | Component: store_sysfs_interface
// File: kernel/xvikil/store/xvk_store_sysfs.c
// Status: Implemented
// Phase: Storage

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/blkdev.h>

static struct kobject *xvk_store_kobj;
static int bfq_low_latency = 1; // Default: 1 (Enabled)

static ssize_t bfq_low_latency_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", bfq_low_latency);
}

static ssize_t bfq_low_latency_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;

    bfq_low_latency = val;
    
    // Note: Directly tuning BFQ queues requires iterating over request queues.
    // For XVK Phase 5, we expose the sysfs node. A userspace script will apply 
    // this to /sys/block/sda/queue/iosched/low_latency.
    printk(KERN_INFO "XVK-STORE: BFQ low latency preference set to %d\n", bfq_low_latency);

    return count;
}

static struct kobj_attribute bfq_low_latency_attr = __ATTR(bfq_low_latency, 0660, bfq_low_latency_show, bfq_low_latency_store);

static int __init xvk_store_sysfs_init(void) {
    xvk_store_kobj = kobject_create_and_add("xvk_store", kernel_kobj);
    if (!xvk_store_kobj)
        return -ENOMEM;

    if (sysfs_create_file(xvk_store_kobj, &bfq_low_latency_attr.attr)) {
        printk(KERN_ERR "XVK-STORE: Failed to create sysfs entry\n");
        kobject_put(xvk_store_kobj);
        return -ENOMEM;
    }

    printk(KERN_INFO "XVK-STORE: Storage sysfs interface initialized\n");
    return 0;
}

static void __exit xvk_store_sysfs_exit(void) {
    sysfs_remove_file(xvk_store_kobj, &bfq_low_latency_attr.attr);
    kobject_put(xvk_store_kobj);
}

module_init(xvk_store_sysfs_init);
module_exit(xvk_store_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Storage Sysfs Interface");
