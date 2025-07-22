#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

// 定时器周期（单位：秒）
#define TASKLET_TIMER_PERIOD 1  // T1: 1秒
#define WORKQUEUE_TIMER_PERIOD 2  // T2: 2秒

// 声明定时器
static struct timer_list tasklet_timer;
static struct timer_list workqueue_timer;

// 声明工作队列
static struct workqueue_struct *my_workqueue;
static struct work_struct my_work;

// 声明tasklet
static struct tasklet_struct my_tasklet;

// tasklet处理函数
static void tasklet_handler(unsigned long data)
{
    printk(KERN_INFO "timer_driver: tasklet executed at jiffies=%lu\n", jiffies);
}

// 工作队列处理函数
static void workqueue_handler(struct work_struct *work)
{
    printk(KERN_INFO "timer_driver: workqueue executed at jiffies=%lu\n", jiffies);
}

// tasklet定时器回调函数
static void tasklet_timer_callback(unsigned long data)
{
    printk(KERN_INFO "timer_driver: tasklet timer callback at jiffies=%lu\n", jiffies);
    
    // 调度tasklet执行
    tasklet_schedule(&my_tasklet);
    
    // 重新设置定时器
    mod_timer(&tasklet_timer, jiffies + TASKLET_TIMER_PERIOD * HZ);
}

// 工作队列定时器回调函数
static void workqueue_timer_callback(unsigned long data)
{
    printk(KERN_INFO "timer_driver: workqueue timer callback at jiffies=%lu\n", jiffies);
    
    // 调度工作队列执行
    queue_work(my_workqueue, &my_work);
    
    // 重新设置定时器
    mod_timer(&workqueue_timer, jiffies + WORKQUEUE_TIMER_PERIOD * HZ);
}

// 初始化函数
static int __init timer_driver_init(void)
{
    printk(KERN_INFO "timer_driver: initializing module\n");
    
    // 初始化tasklet
    tasklet_init(&my_tasklet, tasklet_handler, 0);
    printk(KERN_INFO "timer_driver: tasklet initialized\n");
    
    // 创建工作队列
    my_workqueue = create_workqueue("my_workqueue");
    if (!my_workqueue) {
        printk(KERN_ERR "timer_driver: failed to create workqueue\n");
        return -ENOMEM;
    }
    INIT_WORK(&my_work, workqueue_handler);
    printk(KERN_INFO "timer_driver: workqueue created\n");
    
    // 初始化并启动tasklet定时器
    init_timer(&tasklet_timer);
    tasklet_timer.function = tasklet_timer_callback;
    tasklet_timer.data = 0;
    tasklet_timer.expires = jiffies + TASKLET_TIMER_PERIOD * HZ;
    add_timer(&tasklet_timer);
    printk(KERN_INFO "timer_driver: tasklet timer started with period %d seconds\n", TASKLET_TIMER_PERIOD);
    
    // 初始化并启动工作队列定时器
    init_timer(&workqueue_timer);
    workqueue_timer.function = workqueue_timer_callback;
    workqueue_timer.data = 0;
    workqueue_timer.expires = jiffies + WORKQUEUE_TIMER_PERIOD * HZ;
    add_timer(&workqueue_timer);
    printk(KERN_INFO "timer_driver: workqueue timer started with period %d seconds\n", WORKQUEUE_TIMER_PERIOD);
    
    printk(KERN_INFO "timer_driver: module loaded successfully\n");
    return 0;
}

// 退出函数
static void __exit timer_driver_exit(void)
{
    printk(KERN_INFO "timer_driver: unloading module\n");
    
    // 删除定时器
    del_timer_sync(&tasklet_timer);
    del_timer_sync(&workqueue_timer);
    printk(KERN_INFO "timer_driver: timers deleted\n");
    
    // 销毁tasklet
    tasklet_kill(&my_tasklet);
    printk(KERN_INFO "timer_driver: tasklet killed\n");
    
    // 刷新并销毁工作队列
    flush_workqueue(my_workqueue);
    destroy_workqueue(my_workqueue);
    printk(KERN_INFO "timer_driver: workqueue destroyed\n");
    
    printk(KERN_INFO "timer_driver: module unloaded successfully\n");
}

module_init(timer_driver_init);
module_exit(timer_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK472");
MODULE_DESCRIPTION("A simple timer driver with tasklet and workqueue"); 