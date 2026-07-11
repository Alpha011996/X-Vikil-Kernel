// SUBSYSTEM: XVK-LAB | Component: custom_idle_governor
// File: kernel/xvikil/lab/xvk_idle_gov.c
// Status: Experimental
// Phase: Lab

#include <linux/cpuidle.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static int idle_bias = 0; // 0=Default, 1=Performance (shallow states)

static int xvk_idle_select(struct cpuidle_driver *drv, struct cpuidle_device *dev,
                           bool *stop_tick)
{
    if (idle_bias) {
        // Bias towards state 0 (shallowest) for lower wake latency
        return 0; 
    }
    // Fallback to default menu behavior (simplified for stub)
    return cpuidle_select(drv, dev, stop_tick);
}

static struct cpuidle_governor xvk_idle_governor = {
    .name =         "xvk_idle",
    .rating =       30, // Lower than menu (60)
    .enable =       NULL,
    .select =       xvk_idle_select,
    .reflect =      NULL,
};

static ssize_t idle_bias_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", idle_bias);
}

static ssize_t idle_bias_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;
    idle_bias = val;
    return count;
}

static struct kobj_attribute idle_bias_attr = __ATTR(idle_bias, 0660, idle_bias_show, idle_bias_store);

static int __init xvk_idle_init(void) {
    int ret = cpuidle_register_governor(&xvk_idle_governor);
    if (ret)
        return ret;
    
    struct kobject *xvk_lab_kobj = kobject_create_and_add("xvk_lab", kernel_kobj);
    if (!xvk_lab_kobj)
        return -ENOMEM;
        
    return sysfs_create_file(xvk_lab_kobj, &idle_bias_attr.attr);
}

module_init(xvk_idle_init);
MODULE_LICENSE("GPL");
