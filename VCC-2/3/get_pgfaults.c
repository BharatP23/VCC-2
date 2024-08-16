#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h> 

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Showing the number of Page faults");

//Using all_vm_events to for calculating the number of page fault
static int pgfaults_proc_show(struct seq_file *m, void *v)
{
    unsigned long number_of_page_faults;  
    unsigned long ev[NR_VM_EVENT_ITEMS];
    all_vm_events(ev);
    number_of_page_faults= ev[PGFAULT];
    seq_printf(m, "Page Faults - %lu\n", number_of_page_faults);
    return 0;
}


static int pgfaults_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, pgfaults_proc_show, NULL);
}

//proc file operations
static const struct proc_ops pgfaults_proc_fops = {
    .proc_open = pgfaults_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init pgfaults_init(void)
{

    struct proc_dir_entry *new_entry;
    //Creating a new proc file entry
    new_entry = proc_create("get_pgfaults", 0, NULL, &pgfaults_proc_fops);
    if (!new_entry) {
        printk(KERN_INFO"Proc entry cannot be created\n");
        return -ENOMEM;
    }

    printk(KERN_INFO"get_pgfaults module loaded\n");
    return 0;
}

static void __exit pgfaults_exit(void)
{
    remove_proc_entry("get_pgfaults", NULL);
    printk(KERN_INFO" get_pgfaults module unloaded\n");
}

module_init(pgfaults_init);
module_exit(pgfaults_exit);


