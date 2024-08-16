#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Check if virtual address is mapped to physical address");
MODULE_AUTHOR("Your Name");

unsigned long param_address = 0;
int param_pid = -1;
module_param(param_pid, int, 0);
module_param(param_address, ulong, 0);

//This function is doing the page table walk for the particular pid 
static int module_check_mapping_init(void) {
    struct task_struct *mapping_task;
    for_each_process(mapping_task)
    {
        if(mapping_task->pid == param_pid)
        {
            printk(KERN_INFO "Loading module\n");

            pgd_t *global_enrty = pgd_offset(mapping_task->mm, param_address);

            if (pgd_none(*global_enrty) || pgd_bad(*global_enrty)) {
                printk(KERN_ERR "Not found global_entry\n");
                return 0;
            }


            p4d_t *level_4_entry = p4d_offset(global_enrty, param_address);
            if(p4d_none(*level_4_entry))
            {
                printk(KERN_ERR "Not Found level_4_entry\n");
                return 0;
            }

            pud_t *upper_entry = pud_offset(level_4_entry, param_address);
            if(pud_none(*upper_entry))
            {
                printk(KERN_ERR "Not found upper_entry\n");
                return 0;
            }

            pmd_t *middle_entry= pmd_offset(upper_entry, param_address);
            if(pmd_none(*middle_entry))
            {
                printk(KERN_ERR "Not Found middle_entry\n");
                return 0;
            }

            pte_t *main_entry = pte_offset_kernel(middle_entry, param_address);
            if(pte_none(*main_entry))
            {
                printk(KERN_ERR "Not Found main_entry\n");
                return 0;
            }
            
            int virtual_offset = (~PAGE_MASK) & param_address;
           
            int page_to_frame_translation = pte_val(*main_entry) & PAGE_MASK;
         
            long int virtual_to_physical_mapped_address = page_to_frame_translation | virtual_offset;

            printk(KERN_INFO"Physical Address for Given virtual address is: %lu\n", virtual_to_physical_mapped_address);
           
    }
    }
     return 0; 
}
     

static void module_check_mapping_exit(void) {
    printk(KERN_INFO "Removing module\n");
}

module_init(module_check_mapping_init);
module_exit(module_check_mapping_exit);
