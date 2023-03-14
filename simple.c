#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>


/* This function is called when the module is loaded. */
char* student_id = "B10902222";
int simple_init(void)
{
    printk(KERN_INFO "Loading Module\n");
    printk(KERN_INFO "Student ID: %s\n", student_id); // add this line
    return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void)
{
    printk(KERN_INFO "Removing Module\n");
    printk(KERN_INFO "Student ID: %s\n", student_id); // add this line
}
/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");