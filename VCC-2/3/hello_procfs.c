#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PROC_ENTRY_NAME "hello_procfs"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharat Patidar");
MODULE_DESCRIPTION("Procfs module");

//Printing the message 
static int hello_procfs_show(struct seq_file *m, void *v) {
    seq_printf(m,"Hello World!\n");
    return 0;
}

static int hello_procfs_open(struct inode *inode, struct file *file) {
    return single_open(file, hello_procfs_show, NULL);
}

//Proc file operations
static const struct proc_ops hello_procfs_ops = {
    .proc_open = hello_procfs_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int hello_procfs_init(void) {
  
    struct proc_dir_entry *new_entry;

    //creating new proc entry use proc_create
    new_entry = proc_create(PROC_ENTRY_NAME, 0, NULL, &hello_procfs_ops);
    if (!new_entry) {
        printk(KERN_INFO"Proc entry cannot be created\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "hello_procfs module loaded\n");

    return 0;
}

static void  hello_procfs_exit(void) {
    remove_proc_entry(PROC_ENTRY_NAME, NULL);
    printk(KERN_INFO"hello_procfs module unloaded\n");
}

module_init(hello_procfs_init);
module_exit(hello_procfs_exit);


