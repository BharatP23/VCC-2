#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/pid.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Print process child id");
MODULE_AUTHOR("Bharat Patidar");

int process_pid = -1;
module_param(process_pid, int, 0);

//This function is iterating over the list and for each process printing its pid and state
static void print_children(struct task_struct *parent) {
    struct task_struct *children_task;
    struct list_head *children_list;

    list_for_each(children_list, &parent->children) {
        children_task = list_entry(children_list, struct task_struct, sibling);

        printk(KERN_INFO "Process id is : %d, Process state is %d", children_task->pid, children_task->__state);

        print_children(children_task);
    }
}

//In this module first finding the task struct using the process id 
static int module_print_pids_init(void) {
    struct task_struct *parent_task;

    printk(KERN_INFO "Loading module\n");

    parent_task = pid_task(find_vpid(process_pid), PIDTYPE_PID);

    print_children(parent_task);

    return 0;
}

static void module_print_pids_exit(void) {
    printk(KERN_INFO "Removing module\n");
}

module_init(module_print_pids_init);
module_exit(module_print_pids_exit);
