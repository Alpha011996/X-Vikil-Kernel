// SUBSYSTEM: XVK-INSTR | Component: procfs_interface
// File: kernel/xvikil/proc/xvikil_proc.c
// Status: Implemented
// Phase: Instrumentation

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>

#define XVK_VERSION "1.0-rc1"
#define XVK_BUILD_ID "volcano-sm7635-ac"

static int xvikil_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "X-Vikil Kernel v%s\n", XVK_VERSION);
    seq_printf(m, "Build ID: %s\n", XVK_BUILD_ID);
    seq_printf(m, "Base Kernel: %s\n", UTS_RELEASE);
    return 0;
}

static int xvikil_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, xvikil_proc_show, NULL);
}
static const struct proc_ops xvikil_proc_fops = {
    .proc_open = xvikil_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init xvikil_proc_init(void) {
    proc_create("xvikil/version", 0, NULL, &xvikil_proc_fops);
    printk(KERN_INFO "XVK-INSTR: /proc/xvikil/version initialized\n");
    return 0;
}

static void __exit xvikil_proc_exit(void) {
    remove_proc_entry("xvikil/version", NULL);
}

module_init(xvikil_proc_init);
module_exit(xvikil_proc_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("XVK-RT");
MODULE_DESCRIPTION("X-Vikil Kernel Procfs Interface");
