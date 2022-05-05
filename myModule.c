/*************************************************************************
	> File Name: myModule.c
	> Author: wangkeqi
	> Mail: wangkeqi201005@163.com 
	> Created Time: Mon Mar 28 15:51:33 2022
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/gfp.h>

static int order = 0;
static int count = 1024;
module_param(order, int, 0644);

static int __init myModule_init(void)
{
	struct page *page;

	printk(KERN_INFO"myModule init, order = %d \r\n", order);
	while(count)
	{
		page = __alloc_pages_nodemask(GFP_KERNEL, order, 0, 0);
		count--;
	}
	return 0;
}

static void __exit myModule_exit(void)
{
    printk(KERN_INFO"myModule exit.\r\n");
}

module_init(myModule_init);
module_exit(myModule_exit);

MODULE_LICENSE("GPL");

