#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Printing Process id");
MODULE_AUTHOR("Bharat Patidar");

//This function is use to iterate task struct and check for the process running 
int print_process_id_module_init(void) {
	printk(KERN_INFO "%s", "Print process id modules loaded\n");

	struct task_struct *print_task;

	for_each_process(print_task) {
		 if (print_task->__state == TASK_RUNNING || print_task->__state == TASK_INTERRUPTIBLE || print_task->__state == TASK_UNINTERRUPTIBLE)
			printk(KERN_INFO "Process ID: %d ", print_task->pid);
	}

	return 0;
}

void print_process_id_module_exit(void) {
	printk(KERN_INFO "Module Removed\n");
}

module_init(print_process_id_module_init);
module_exit(print_process_id_module_exit);