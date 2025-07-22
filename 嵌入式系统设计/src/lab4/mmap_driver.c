#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mmap_device"
#define DEVICE_MAJOR 241
#define DEVICE_MINOR 0

// 64KB内存 = 16个4KB页面
#define NPAGES 16
#define MMAP_SIZE (NPAGES * PAGE_SIZE) // 64KB

// 全局变量
static unsigned long kernel_mem_addr = 0;  // 内核虚拟地址
static dev_t dev_num;                      // 设备号
static struct cdev mmap_cdev;              // 字符设备

// mmap 故障处理函数
static int mmap_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
    struct page *page;
    unsigned long offset, addr;
    
    offset = (unsigned long)vmf->virtual_address - (unsigned long)vma->vm_start;
    printk(KERN_INFO "mmap_driver: fault handler called for offset %lu (page %lu)\n", 
           offset, offset / PAGE_SIZE);
    
    // 检查偏移是否超出范围
    if (offset >= MMAP_SIZE) {
        printk(KERN_ERR "mmap_driver: invalid offset in fault handler: %lu\n", offset);
        return VM_FAULT_SIGBUS;
    }
    
    // 计算内核态逻辑地址
    addr = (unsigned long)(kernel_mem_addr + offset);
    printk(KERN_INFO "mmap_driver: mapping kernel addr 0x%lx to user virtual addr 0x%lx\n", 
           addr, (unsigned long)vmf->virtual_address);
    
    // 获取页结构
    page = virt_to_page((void *)addr);
    get_page(page);
    
    vmf->page = page;
    
    return 0;
}
// 虚拟内存操作结构体
static struct vm_operations_struct mmap_vm_ops = {
    .fault = mmap_fault,
};

// 处理用户空间的 mmap() 系统调用
static int mmap_drv_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long size = vma->vm_end - vma->vm_start;
    
    printk(KERN_INFO "mmap_driver: mmap called, size: %lu KB\n", size / 1024);
    
    // 检查映射大小
    if (size > MMAP_SIZE) {
        printk(KERN_ERR "mmap_driver: requested mapping size %lu exceeds available size %lu\n",
               size, (unsigned long)MMAP_SIZE);
        return -EINVAL;
    }
    
    // 使用vma fault机制
    vma->vm_ops = &mmap_vm_ops; // 故障处理函数
    vma->vm_flags |= VM_RESERVED; // 保证不被交换到磁盘
    
    printk(KERN_INFO "mmap_driver: vma setup complete, waiting for page faults\n");
    
    return 0;
}

// 文件操作结构体
static struct file_operations mmap_fops = {
    .owner = THIS_MODULE,
    .mmap = mmap_drv_mmap,
};

// 初始化函数
static int __init mmap_drv_init(void)
{
    int ret;
    
    printk(KERN_INFO "mmap_driver: initializing module\n");
    
    // 申请物理内存空间 (64KB = 16页，2^4=16，所以order为4)
    kernel_mem_addr = __get_free_pages(GFP_KERNEL, 4);
    if (!kernel_mem_addr) {
        printk(KERN_ERR "mmap_driver: failed to allocate %lu KB memory\n", (unsigned long)MMAP_SIZE / 1024);
        return -ENOMEM;
    }
    
    // 初始化内存为0
    memset((void *)kernel_mem_addr, 0, MMAP_SIZE);
    
    printk(KERN_INFO "mmap_driver: allocated %lu KB memory at kernel address 0x%lx\n", 
           (unsigned long)MMAP_SIZE / 1024, kernel_mem_addr);
    
    // 注册字符设备
    dev_num = MKDEV(DEVICE_MAJOR, DEVICE_MINOR);
    ret = register_chrdev_region(dev_num, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "mmap_driver: failed to register device number\n");
        free_pages(kernel_mem_addr, 4);
        return ret;
    }
    
    // 初始化字符设备
    cdev_init(&mmap_cdev, &mmap_fops);
    mmap_cdev.owner = THIS_MODULE;
    
    // 添加字符设备
    ret = cdev_add(&mmap_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "mmap_driver: failed to add device\n");
        unregister_chrdev_region(dev_num, 1);
        free_pages(kernel_mem_addr, 4);
        return ret;
    }
    
    printk(KERN_INFO "mmap_driver: device created with major: %d, minor: %d\n", 
           MAJOR(dev_num), MINOR(dev_num));
    printk(KERN_INFO "mmap_driver: module loaded successfully\n");
    
    return 0;
}

// 退出函数
static void __exit mmap_drv_exit(void)
{
    printk(KERN_INFO "mmap_driver: unloading module\n");
    
    // 移除设备
    cdev_del(&mmap_cdev);
    unregister_chrdev_region(dev_num, 1);
    
    // 释放内存
    if (kernel_mem_addr) {
        printk(KERN_INFO "mmap_driver: freeing %lu KB memory at kernel address 0x%lx\n", 
               (unsigned long)MMAP_SIZE / 1024, kernel_mem_addr);
        free_pages(kernel_mem_addr, 4);
        kernel_mem_addr = 0;
    }
    
    printk(KERN_INFO "mmap_driver: module unloaded successfully\n");
}

module_init(mmap_drv_init);
module_exit(mmap_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MK472");
MODULE_DESCRIPTION("A memory mapping driver using page fault"); 