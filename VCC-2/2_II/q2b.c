#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/signal.h>


#define FIRST_MINOR 0
#define MINOR_CNT 1


// IOCTL commands
#define IOCTL_CHANGE_PARENT _IOW('k', 1, pid_t)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("creating new ioctl driver");


static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

// Function to handle ioctl calls
static long new_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    
    switch (cmd) {
    case IOCTL_CHANGE_PARENT:
        pid_t parent_pid;
        //Receiving the parent process id 
        rcu_read_lock();
        if (copy_from_user(&parent_pid, (struct pid_t*)arg, sizeof(parent_pid)))
            return -EFAULT;
        
        struct task_struct *task_cs;
        task_cs = pid_task(find_vpid(parent_pid), PIDTYPE_PID);

        struct list_head *position, *n;
        list_for_each(position, &current->parent->children) {
            struct task_struct *task_entry = list_entry(position, struct task_struct, sibling);

            if (task_entry == current) {
                n = position->next;
                list_del(position);
                position = n;
                break;
            }
        }
        current->parent = task_cs;
        current->real_parent=task_cs;
        list_add_tail(&current->sibling, &task_cs->children);
        rcu_read_unlock();
        break;

    default:
        return -ENOTTY; 
    }

    return 0;
}

// File operations structure
static const struct file_operations new_ioctl_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = new_ioctl,
};

// Module initialization
static int __init new_ioctl_init(void) {
    int ret;
    struct device *dev_ret;
 
 
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &new_ioctl_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "parent_change")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}

// Module cleanup
static void __exit new_ioctl_exit(void) {
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(new_ioctl_init);
module_exit(new_ioctl_exit);
