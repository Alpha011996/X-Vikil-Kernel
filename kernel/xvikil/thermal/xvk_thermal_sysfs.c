// SUBSYSTEM: XVK-THERM | Component: thermal_sysfs_interface
// File: kernel/xvikil/thermal/xvk_thermal_sysfs.c
// Status: Implemented
// Phase: Thermal

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/cpufreq.h>
#include <linux/workqueue.h>

static struct kobject *xvk_therm_kobj;
static int predictive_throttle = 1; // 1=Enabled, 0=Disabled
static int thermal_headroom = 20;   // % of max freq to reserve during throttle

static void xvk_throttle_work(struct work_struct *work);
static DECLARE_DELAYED_WORK(xvk_throttle_workq, xvk_throttle_work);

static void xvk_throttle_work(struct work_struct *work) {
    struct cpufreq_policy *policy;
    unsigned int max_freq;
    int cpu;

    for_each_possible_cpu(cpu) {
        policy = cpufreq_cpu_get(cpu);
        if (!policy)
            continue;

        if (predictive_throttle) {
            max_freq = policy->cpuinfo.max_freq * (100 - thermal_headroom) / 100;
        } else {
            max_freq = policy->cpuinfo.max_freq;
        }
        cpufreq_update_policy(cpu); // Refresh policy
        cpufreq_cpu_put(policy);
    }
    schedule_delayed_work(&xvk_throttle_workq, msecs_to_jiffies(1000));
}

static ssize_t predictive_throttle_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", predictive_throttle);
}

static ssize_t predictive_throttle_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;

    predictive_throttle = val;
    printk(KERN_INFO "XVK-THERM: Predictive throttle set to %d\n", predictive_throttle);

    return count;
}

static ssize_t thermal_headroom_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", thermal_headroom);
}

static ssize_t thermal_headroom_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;

    if (val < 0 || val > 80)
        return -EINVAL;

    thermal_headroom = val;
    printk(KERN_INFO "XVK-THERM: Thermal headroom set to %d%%\n", thermal_headroom);

    return count;
}

static struct kobj_attribute predictive_throttle_attr = __ATTR(predictive_throttle, 0660, predictive_throttle_show, predictive_throttle_store);
static struct kobj_attribute thermal_headroom_attr = __ATTR(thermal_headroom, 0660, thermal_headroom_show, thermal_headroom_store);

static int __init xvk_thermal_sysfs_init(void) {
    xvk_therm_kobj = kobject_create_and_add("xvk_therm", kernel_kobj);
    if (!xvk_therm_kobj)
        return -ENOMEM;

    if (sysfs_create_file(xvk_therm_kobj, &predictive_throttle_attr.attr) ||
        sysfs_create_file(xvk_therm_kobj, &thermal_headroom_attr.attr)) {
        printk(KERN_ERR "XVK-THERM: Failed to create sysfs entry\n");
        kobject_put(xvk_therm_kobj);
        return -ENOMEM;
    }

    schedule_delayed_work(&xvk_throttle_workq, msecs_to_jiffies(1000));
    printk(KERN_INFO "XVK-THERM: Thermal sysfs interface initialized\n");
    return 0;
}

static void __exit xvk_thermal_sysfs_exit(void) {
    cancel_delayed_work_sync(&xvk_throttle_workq);
    sysfs_remove_file(xvk_therm_kobj, &predictive_throttle_attr.attr);
    sysfs_remove_file(xvk_therm_kobj, &thermal_headroom_attr.attr);
    kobject_put(xvk_therm_kobj);
}

module_init(xvk_thermal_sysfs_init);
module_exit(xvk_thermal_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Thermal Sysfs Interface");
