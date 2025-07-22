#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define PROC_NAME "buffer_proc"  // proc文件名称

// 定义模块参数N（缓冲区大小）
static int buffer_size = 1024;  // 默认大小为1024字节
module_param(buffer_size, int, S_IRUGO);
MODULE_PARM_DESC(buffer_size, "缓冲区大小");

// 全局字符指针
static char *buffer_ptr = NULL;

// proc文件的读取函数
static ssize_t buffer_proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
    size_t len;
    
    printk(KERN_INFO "simple_proc_driver: proc read called\n");
    
    if (*ppos >= buffer_size)
        return 0;
    
    len = buffer_size - *ppos;
    if (len > count)
        len = count;
    
    if (copy_to_user(user_buf, buffer_ptr + *ppos, len)) {
        printk(KERN_ERR "simple_proc_driver: copy_to_user failed\n");
        return -EFAULT;
    }
    
    *ppos += len;
    printk(KERN_INFO "simple_proc_driver: %zu bytes read\n", len);
    
    return len;
}

// proc文件的写入函数
static ssize_t buffer_proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
    size_t len;
    
    printk(KERN_INFO "simple_proc_driver: proc write called\n");
    
    if (*ppos >= buffer_size)
        return -ENOSPC;
    
    len = buffer_size - *ppos;
    if (len > count)
        len = count;
    
    if (copy_from_user(buffer_ptr + *ppos, user_buf, len)) {
        printk(KERN_ERR "simple_proc_driver: copy_from_user failed\n");
        return -EFAULT;
    }
    
    *ppos += len;
    printk(KERN_INFO "simple_proc_driver: %zu bytes written\n", len);
    
    return len;
}

static const struct file_operations proc_file_ops = {
    .read = buffer_proc_read,
    .write = buffer_proc_write,
};

static struct proc_dir_entry *proc_file_entry;

// 模块初始化函数
static int __init simple_proc_init(void)
{
    printk(KERN_INFO "simple_proc_driver: initializing module with buffer size %d\n", buffer_size);
    
    // 分配内存
    buffer_ptr = kmalloc(buffer_size, GFP_KERNEL);
    if (!buffer_ptr) {
        printk(KERN_ERR "simple_proc_driver: failed to allocate memory\n");
        return -ENOMEM;
    }
    
    // 初始化缓冲区为0
    memset(buffer_ptr, 0, buffer_size);
    
    // 创建proc文件
    proc_file_entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    if (!proc_file_entry) {
        printk(KERN_ERR "simple_proc_driver: failed to create proc entry\n");
        kfree(buffer_ptr);
        return -ENOMEM;
    }
    
    printk(KERN_INFO "simple_proc_driver: module loaded successfully\n");
    printk(KERN_INFO "simple_proc_driver: proc file created at /proc/%s\n", PROC_NAME);
    
    return 0;
}

// 模块卸载函数
static void __exit simple_proc_exit(void)
{
    printk(KERN_INFO "simple_proc_driver: unloading module\n");
    
    // 移除proc文件
    remove_proc_entry(PROC_NAME, NULL);
    
    // 释放内存
    if (buffer_ptr) {
        printk(KERN_INFO "simple_proc_driver: freeing buffer memory\n");
        kfree(buffer_ptr);
        buffer_ptr = NULL;
    }
    
    printk(KERN_INFO "simple_proc_driver: module unloaded successfully\n");
}

module_init(simple_proc_init);
module_exit(simple_proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK472");
MODULE_DESCRIPTION("A simple proc file system driver"); 