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
#include <linux/slab.h>

static int order = 10;
//static int count = 100;
//module_param(order, int, 0644);
#define count 10

//static void *p_array[count] = {0};
struct page *p_array[count] = {0};

static int __init myModule_init(void)
{
	int a = 0;
	printk(KERN_INFO"myModule init: order = %d \r\n", order);
	for(a = 0; a < count; a++)
	{
		//p_array[a] = kmalloc(4*1024*1024, GFP_KERNEL);
		p_array[a] = __alloc_pages_node(0,
                     (GFP_HIGHUSER_MOVABLE |
                      __GFP_THISNODE | __GFP_NOMEMALLOC |
                      __GFP_NORETRY | __GFP_NOWARN | __GFP_DMA) &
                     ~__GFP_RECLAIM, 10);
		printk(KERN_INFO"myModule init: p = %lx \r\n",(long unsigned int)p_array[a]);
	}
	return 0;
}

static void __exit myModule_exit(void)
{
	int a = 0;
    printk(KERN_INFO"myModule exit.\r\n");
	for(a = 0; a < count; a++)
	{
		if(p_array[a] == NULL)
			continue;
		__free_pages(p_array[a],10);
		//kfree(p_array[a]);
	}
}

module_init(myModule_init);
module_exit(myModule_exit);

MODULE_LICENSE("GPL");

