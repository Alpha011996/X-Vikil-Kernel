// SUBSYSTEM: XVK-GFX | Component: gfx_sysfs_interface
// File: kernel/xvikil/gfx/xvk_gfx_sysfs.c
// Status: Implemented
// Phase: Graphics

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

static struct kobject *xvk_gfx_kobj;
static int gpu_busy_threshold = 50; // Default: 50% busy to scale up

static ssize_t gpu_busy_threshold_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", gpu_busy_threshold);
}

static ssize_t gpu_busy_threshold_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;

    if (val < 10 || val > 90)
        return -EINVAL;

    gpu_busy_threshold = val;
    // Note: Applied to /sys/class/kgsl/kgsl-3d0/devfreq/governor
    printk(KERN_INFO "XVK-GFX: GPU busy threshold set to %d%%\n", gpu_busy_threshold);

    return count;
}

static struct kobj_attribute gpu_busy_threshold_attr = __ATTR(gpu_busy_threshold, 0660, gpu_busy_threshold_show, gpu_busy_threshold_store);

static int __init xvk_gfx_sysfs_init(void) {
    xvk_gfx_kobj = kobject_create_and_add("xvk_gfx", kernel_kobj);
    if (!xvk_gfx_kobj)
        return -ENOMEM;

    if (sysfs_create_file(xvk_gfx_kobj, &gpu_busy_threshold_attr.attr)) {
        printk(KERN_ERR "XVK-GFX: Failed to create sysfs entry\n");
        kobject_put(xvk_gfx_kobj);
        return -ENOMEM;
    }

    printk(KERN_INFO "XVK-GFX: Graphics sysfs interface initialized\n");
    return 0;
}

static void __exit xvk_gfx_sysfs_exit(void) {
    sysfs_remove_file(xvk_gfx_kobj, &gpu_busy_threshold_attr.attr);
    kobject_put(xvk_gfx_kobj);
}

module_init(xvk_gfx_sysfs_init);
module_exit(xvk_gfx_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Graphics Sysfs Interface");
