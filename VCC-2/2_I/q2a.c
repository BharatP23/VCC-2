#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include<asm-generic/io.h>


#define FIRST_MINOR 0
#define MINOR_CNT 1


#define IOCTL_GET_PHYSICAL_ADDRESS _IOWR('k', 1, struct GetInfo)
#define IOCTL_WRITE_PHYSICAL_ADDRESS _IOWR('k', 2, struct WriteInfo)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharat Patidar");
MODULE_DESCRIPTION("Creating new ioctl");

struct WriteInfo {
    unsigned long physical_address;
    char value;
};
struct GetInfo {
    unsigned long virtual_address;
    unsigned long physical_address;
};

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

//In this function we are doing page table walk to get the mapped physical address 
static long get_physical_address(long int virtual_address)
{
        struct task_struct *task;

        pgd_t *global_enrty = pgd_offset(current->mm, virtual_address);

        if (pgd_none(*global_enrty) || pgd_bad(*global_enrty)) {
            printk(KERN_ERR "Not found global_entry\n");
            return 0;
        }


        p4d_t *level_4_entry = p4d_offset(global_enrty, virtual_address);
        if(p4d_none(*level_4_entry))
        {
            printk(KERN_ERR "Not Found level_4_entry\n");
            return 0;
        }

        pud_t *upper_entry = pud_offset(level_4_entry, virtual_address);
        if(pud_none(*upper_entry))
        {
            printk(KERN_ERR "Not found upper_entry\n");
            return 0;
        }

        pmd_t *middle_entry= pmd_offset(upper_entry, virtual_address);
        if(pmd_none(*middle_entry))
        {
            printk(KERN_ERR "Not Found middle_entry\n");
            return 0;
        }

        pte_t *main_entry = pte_offset_kernel(middle_entry, virtual_address);
        if(pte_none(*main_entry))
        {
            printk(KERN_ERR "Not Found main_entry\n");
            return 0;
        }
        
        //Taking offset bit from the virtual address
        int virtual_offset = (~PAGE_MASK) & virtual_address;
        
        //Taking the frame entry address from the page table entry
        int page_to_frame_translation = pte_val(*main_entry) & PAGE_MASK;
        
        //In this we are combining the offset from virtual address and the frame address to get physical address
        long int virtual_to_physical_mapped_address = page_to_frame_translation | virtual_offset;

        printk(KERN_INFO"Physical Address for Given virtual address is: %lu\n", virtual_to_physical_mapped_address);

        return virtual_to_physical_mapped_address;
}


static long new_ioctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
   
    struct WriteInfo write_info;
    struct GetInfo get_info;
    switch (cmd) {
    //This case is used to get the physical address which is mapped to virtual adddress
    case IOCTL_GET_PHYSICAL_ADDRESS:
        if (copy_from_user(&get_info, (struct GetInfo*)arg, sizeof(get_info)))
            return -EFAULT;

        get_info.physical_address = get_physical_address(get_info.virtual_address);

        printk(KERN_INFO "Received Virtual Address is : 0x%lx\n", get_info.virtual_address);
        printk(KERN_INFO "Mapped Physical Address is : 0x%lx\n", get_info.physical_address);

        if (copy_to_user((struct GetInfo*)arg, &get_info, sizeof(get_info)))
            return -EFAULT;
        break;

    //This Case is used to write value on the physical address by converting the physical address to virtual
    case IOCTL_WRITE_PHYSICAL_ADDRESS:
        if (copy_from_user(&write_info, (struct WriteInfo *)arg, sizeof(struct WriteInfo)))
            return -EFAULT;
        printk(KERN_INFO "Received physical address: 0x%lx, value: %d\n", write_info.physical_address, write_info.value);
    
        char *virtt = phys_to_virt(write_info.physical_address);
        *virtt=write_info.value;
    
        break;
    default:
        return -ENOTTY; 
    }

    return 0;
}

// Proc file operation
static const struct file_operations new_ioctl_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = new_ioctl_ioctl,
};


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
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "my_ioctl")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}


static void __exit new_ioctl_exit(void) {
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(new_ioctl_init);
module_exit(new_ioctl_exit);
