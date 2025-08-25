#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sysinfo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maiquel Paiva");
MODULE_DESCRIPTION("Simple system monitor via kernel");

#define PROC_NAME "sys_monitor"

static struct proc_dir_entry *proc_entry;

static int sys_monitor_show(struct seq_file *m, void *v) {
    struct task_struct *task;
    int process_count = 0;
    struct sysinfo i;

    si_meminfo(&i);

    for_each_process(task) {
        process_count++;
    }

    seq_printf(m, "Active processes: %d\n", process_count);
    seq_printf(m, "Total memory: %lu bytes\n", i.totalram << PAGE_SHIFT);
    seq_printf(m, "Free memory: %lu bytes\n", i.freeram << PAGE_SHIFT);

    return 0;
}

static int sys_monitor_open(struct inode *inode, struct file *file) {
    return single_open(file, sys_monitor_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = sys_monitor_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

static int __init sys_monitor_init(void) {
    printk(KERN_INFO "SysMonitor: initializing module\n");
    proc_entry = proc_create(PROC_NAME, 0, NULL, &proc_fops);
    if (!proc_entry) {
        printk(KERN_ALERT "SysMonitor: failed to create /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit sys_monitor_exit(void) {
    proc_remove(proc_entry);
    printk(KERN_INFO "SysMonitor: module removed\n");
}

module_init(sys_monitor_init);
module_exit(sys_monitor_exit);
