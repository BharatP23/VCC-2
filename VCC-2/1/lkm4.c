#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/pid.h>

MODULE_LICENSE("GPL");
int process_id = -1;
module_param(process_id, int, 0);

//This function is first taking task_struct of the process whose pid is passed as argument
//Then using total_vm function to find the allocated memory
static int calculating_total_vma_size(unsigned int pid) {
    printk(KERN_INFO "LKM4: Module loaded.\n");

    struct task_struct *process_task;
    struct mm_struct *task_mm;

    process_task = get_pid_task(find_get_pid(process_id), PIDTYPE_PID);
    if (!process_task) {
        pr_info("Incorrect process ID\n");
        return -EINVAL;
    }

    task_mm = process_task->mm;
    if (!task_mm) {
        pr_info("No memory mapping found\n");
        return -EINVAL;
    }

    printk(KERN_INFO "Total Virtual address space = %d KB", task_mm->total_vm * 4);

    //Calculating total mapped physical memory
    int count_mapped_physical_address_space = 0;

    count_mapped_physical_address_space += atomic_long_read(&task_mm->rss_stat.count[MM_FILEPAGES]);

    count_mapped_physical_address_space += atomic_long_read(&task_mm->rss_stat.count[MM_ANONPAGES]);

    count_mapped_physical_address_space += atomic_long_read(&task_mm->rss_stat.count[MM_SHMEMPAGES]);

    count_mapped_physical_address_space += atomic_long_read(&task_mm->rss_stat.count[MM_SWAPENTS]);

    printk(KERN_INFO "Total mapped physical address space = %d KB\n", count_mapped_physical_address_space * 4);

    return 0;
}

static int __init lkm4_init(void) {
    
    calculating_total_vma_size(process_id);
    return 0;
}

static void __exit lkm4_exit(void) {
    pr_info("LKM unloaded\n");
}

module_init(lkm4_init);
module_exit(lkm4_exit);
