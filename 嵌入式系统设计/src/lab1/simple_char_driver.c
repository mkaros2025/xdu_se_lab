#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "simple_char_dev"
#define DEVICE_MAJOR 240
#define DEVICE_MINOR 0

// 定义数据结构
struct simple_dev_data {
    char buffer[1024];
    int count;
    size_t size;  // 添加这一行，记录实际数据大小
};

// 定义全局的数据结构指针，初始化为NULL
static struct simple_dev_data *dev_data = NULL;

// 打开设备文件时调用
static int simple_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_driver: device opened\n");
    
    if (dev_data) {
        dev_data->count++;
        printk(KERN_INFO "simple_char_driver: incrementing count to %d\n", dev_data->count);
    }
    
    return 0;
}

// 释放设备文件时调用
static int simple_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "simple_char_driver: device released\n");
    
    if (dev_data) {
        dev_data->count--;
        printk(KERN_INFO "simple_char_driver: decremented count to %d\n", dev_data->count);
    }
    
    return 0;
}

// 读取设备文件时调用
static ssize_t simple_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    size_t bytes_to_read;
    
    printk(KERN_INFO "simple_char_driver: read operation called\n");
    
    if (dev_data == NULL) {
        printk(KERN_ERR "simple_char_driver: device data not allocated\n");
        return -EINVAL;
    }
    
    // 只读取到实际写入的数据大小
    if (*ppos >= dev_data->size)
        return 0;
    
    // 计算可用数据量
    bytes_to_read = dev_data->size - *ppos;
    if (bytes_to_read > count)
        bytes_to_read = count;
    
    // 将数据从内核空间复制到用户空间
    if (copy_to_user(buf, dev_data->buffer + *ppos, bytes_to_read)) {
        printk(KERN_ERR "simple_char_driver: failed to copy data to user\n");
        return -EFAULT;
    }
    
    // 更新文件位置
    *ppos += bytes_to_read;
    printk(KERN_INFO "simple_char_driver: %zu bytes read out of %zu total bytes\n", 
           bytes_to_read, dev_data->size);
    
    return bytes_to_read;
}

// 写入设备文件时调用
static ssize_t simple_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    size_t bytes_to_write;
    
    printk(KERN_INFO "simple_char_driver: write operation called\n");
    
    if (dev_data == NULL) {
        printk(KERN_ERR "simple_char_driver: device data not allocated\n");
        return -EINVAL;
    }
    
    // 写完了就不能写了
    if (*ppos >= sizeof(dev_data->buffer))
        return -ENOSPC;
    
    bytes_to_write = sizeof(dev_data->buffer) - *ppos;
    if (bytes_to_write > count)
        bytes_to_write = count;
    
    if (copy_from_user(dev_data->buffer + *ppos, buf, bytes_to_write)) {
        printk(KERN_ERR "simple_char_driver: failed to copy data from user\n");
        return -EFAULT;
    }
    
    *ppos += bytes_to_write;
    // 更新size为写入位置的最大值
    if (*ppos > dev_data->size)
        dev_data->size = *ppos;
        
    printk(KERN_INFO "simple_char_driver: %zu bytes written, total size now %zu\n", 
           bytes_to_write, dev_data->size);
    
    return bytes_to_write;
}

// 文件操作结构体
static struct file_operations simple_fops = {
    .owner = THIS_MODULE,
    .open = simple_open,
    .release = simple_release,
    .read = simple_read,
    .write = simple_write,
};

// 模块初始化函数
static int __init simple_init(void)
{
    int ret;
    
    printk(KERN_INFO "simple_char_driver: initializing module\n");
    
    // 在这里分配内存
    dev_data = kmalloc(sizeof(struct simple_dev_data), GFP_KERNEL);
    if (!dev_data) {
        printk(KERN_ERR "simple_char_driver: failed to allocate memory\n");
        return -ENOMEM;
    }
    memset(dev_data->buffer, 0, sizeof(dev_data->buffer));
    dev_data->count = 0;  // 初始没有进程打开
    dev_data->size = 0;   // 初始数据大小为0
    
    // 注册字符设备
    ret = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &simple_fops);
    if (ret < 0) {
        printk(KERN_ERR "simple_char_driver: failed to register character device\n");
        kfree(dev_data);  // 失败时释放内存
        return ret;
    }
    
    printk(KERN_INFO "simple_char_driver: module initialized successfully (major: %d)\n", DEVICE_MAJOR);
    
    return 0;
}

// 模块退出函数
static void __exit simple_exit(void)
{
    printk(KERN_INFO "simple_char_driver: unloading module\n");
    
    // 注销字符设备
    unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);
    
    // 释放内存
    if (dev_data) {
        printk(KERN_INFO "simple_char_driver: freeing device data memory\n");
        kfree(dev_data);
        dev_data = NULL;
    }
    
    printk(KERN_INFO "simple_char_driver: module unloaded successfully\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK472");
MODULE_DESCRIPTION("A simple character device driver");