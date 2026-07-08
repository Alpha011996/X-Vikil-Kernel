// SUBSYSTEM: XVK-SEC | Component: sec_sysfs_interface
// File: kernel/xvikil/sec/xvk_sec_sysfs.c
// Status: Implemented
// Phase: Security

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/nsproxy.h>
#include <linux/pid_namespace.h>

static struct kobject *xvk_sec_kobj;
static int namespace_isolation = 1; // Default: 1 (Enforced)

static ssize_t namespace_isolation_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    return sprintf(buf, "%d\n", namespace_isolation);
}

static ssize_t namespace_isolation_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    int ret, val;
    ret = kstrtoint(buf, 10, &val);
    if (ret < 0)
        return ret;

    namespace_isolation = val;
    printk(KERN_INFO "XVK-SEC: Namespace isolation set to %d\n", namespace_isolation);

    return count;
}

static struct kobj_attribute namespace_isolation_attr = __ATTR(namespace_isolation, 0660, namespace_isolation_show, namespace_isolation_store);

static int __init xvk_sec_sysfs_init(void) {
    xvk_sec_kobj = kobject_create_and_add("xvk_sec", kernel_kobj);
    if (!xvk_sec_kobj)
        return -ENOMEM;

    if (sysfs_create_file(xvk_sec_kobj, &namespace_isolation_attr.attr)) {
        printk(KERN_ERR "XVK-SEC: Failed to create sysfs entry\n");
        kobject_put(xvk_sec_kobj);
        return -ENOMEM;
    }

    printk(KERN_INFO "XVK-SEC: Security sysfs interface initialized\n");
    return 0;
}

static void __exit xvk_sec_sysfs_exit(void) {
    sysfs_remove_file(xvk_sec_kobj, &namespace_isolation_attr.attr);
    kobject_put(xvk_sec_kobj);
}

module_init(xvk_sec_sysfs_init);
module_exit(xvk_sec_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Security Sysfs Interface");
