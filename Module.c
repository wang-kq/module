#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/timer.h>

#define count 200
#define TIME 2

static char flag_str = '0';
struct page *p_array[count] = {0};
static struct workqueue_struct *alloc_dma_wq;
static struct work_struct alloc_dma_wk;
static struct timer_list alloc_timer;
static struct proc_dir_entry *p_proc;
int alloc_flag = 0;

static int proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\r\n", &flag_str); 
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_show, NULL);
}

static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t nbytes, loff_t *ppos)
{

	if (copy_from_user(&flag_str, user_buf, 1)) {
		return -1;
	}
	return nbytes;
}

static const struct file_operations proc_operations = {
        .open           = proc_open,
        .read           = seq_read,
        .llseek         = seq_lseek,
		.write          = proc_write,
        .release        = seq_release,
};

static void alloc_timer_function(struct timer_list *ptimer)
{
	//flush_workqueue(alloc_dma_wq);
	schedule_work(&alloc_dma_wk);
	//mod_timer(&alloc_timer, jiffies + HZ);
    //printk("moduel: timer irq \n");
}

static void alloc_timer_register(void)
{
    //printk("module: timer_register!\n");
    alloc_timer.expires = jiffies + (TIME * HZ);
    alloc_timer.function = alloc_timer_function;
    add_timer(&alloc_timer);
}

static void alloc_dma_work_fn(struct work_struct *work)
{
	int a = 0;
	//printk("Module: work in\r\n");
	mod_timer(&alloc_timer, jiffies + (TIME * HZ));
	if (flag_str == '0') {
		for(a = 0; a < count; a++)
		{
			if(p_array[a] != NULL) {
				__free_pages(p_array[a],10);
				p_array[a] = NULL;
			}
		}
		//printk("Module: no alloc \r\n");
		alloc_flag =0;
	} else {
	//printk("Module: alloc \r\n");
		if (alloc_flag == 0) {
			for (a = 0; a < count; a++) {
			    p_array[a] = __alloc_pages_node(0,
			                 (GFP_HIGHUSER_MOVABLE |
			                  __GFP_THISNODE | __GFP_NOMEMALLOC |
			                  __GFP_NORETRY | __GFP_NOWARN | __GFP_DMA) &
			                 ~__GFP_RECLAIM, 10);
			}
			alloc_flag =1;
		}
	}
}

static int __init Module_init(void)
{
	//printk("Module init.\r\n");
	p_proc = proc_create("alloc_dma_page", 0777, NULL, &proc_operations);
	alloc_dma_wq = alloc_workqueue("alloc_dma", 0, 1);
	INIT_WORK(&alloc_dma_wk, alloc_dma_work_fn);
	//queue_work(alloc_dma_wq, &alloc_dma_wk);
    alloc_timer_register();
	return 0;
}

static void __exit Module_exit(void)
{
	int a = 0;
    //printk("Module exit.\r\n");
	for(a = 0; a < count; a++)
	{
		if(p_array[a] != NULL)
			__free_pages(p_array[a],10);
	}
	proc_remove(p_proc);
	destroy_workqueue(alloc_dma_wq);
	del_timer(&alloc_timer);
}

module_init(Module_init);
module_exit(Module_exit);

MODULE_LICENSE("GPL");

