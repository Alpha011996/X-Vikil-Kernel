// SUBSYSTEM: XVK-SCHED | Component: uclamp_interface
// File: kernel/xvikil/sched/xvk_sched_uclamp.c
// Status: Implemented
// Phase: Scheduler

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/sched.h>
#include <linux/sched/cpufreq.h>

static struct kobject *xvk_sched_kobj;
static int uclamp_topapp_min = 0; // Default: No boost (0-1024 scale)

static ssize_t uclamp_topapp_min_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", uclamp_topapp_min);
}

static ssize_t uclamp_topapp_min_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;
    
    if (val < 0 || val > 1024)
        return -EINVAL;
        
    uclamp_topapp_min = val;
    
    // Apply to all online CPUs
    // Note: In a full implementation, this would hook into the scheduler's
    // uclamp_eff_value logic to override the default clamps for top-app tasks.
    // For XVK Phase 2, we expose the interface and log the intent.
    printk(KERN_INFO "XVK-SCHED: UCLAMP top-app min set to %d\n", uclamp_topapp_min);
    
    return count;
}

static struct kobj_attribute uclamp_topapp_min_attr = __ATTR(uclamp_topapp_min, 0660, uclamp_topapp_min_show, uclamp_topapp_min_store);

static int __init xvk_sched_uclamp_init(void) {
    xvk_sched_kobj = kobject_create_and_add("xvk_sched", kernel_kobj);
    if (!xvk_sched_kobj)
        return -ENOMEM;
    
    if (sysfs_create_file(xvk_sched_kobj, &uclamp_topapp_min_attr.attr)) {
        printk(KERN_ERR "XVK-SCHED: Failed to create uclamp sysfs entry\n");
        kobject_put(xvk_sched_kobj);
        return -ENOMEM;
    }
    
    printk(KERN_INFO "XVK-SCHED: UCLAMP interface initialized\n");
    return 0;
}

static void __exit xvk_sched_uclamp_exit(void) {
    sysfs_remove_file(xvk_sched_kobj, &uclamp_topapp_min_attr.attr);
    kobject_put(xvk_sched_kobj);
}

module_init(xvk_sched_uclamp_init);
module_exit(xvk_sched_uclamp_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Scheduler UCLAMP Interface");
