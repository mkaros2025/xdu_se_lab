#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DEVICE_PATH "/dev/mmap_device"
#define MMAP_SIZE (64 * 1024)  // 64 KB
#define PAGE_SIZE 4096

int main()
{
    int fd;
    char *mapped_mem;
    int i, page_index;
    char buffer[100];
    
    printf("测试程序: 打开设备文件...\n");
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("无法打开设备文件");
        return -1;
    }
    printf("测试程序: 设备文件已成功打开\n");
    
    // 映射内存
    printf("测试程序: 映射 %d KB 内存...\n", MMAP_SIZE / 1024);
    mapped_mem = (char *)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_mem == MAP_FAILED) {
        perror("内存映射失败");
        close(fd);
        return -1;
    }
    printf("测试程序: 内存映射成功，地址: %p\n", mapped_mem);
    
    // 写入每个页面的开始位置，触发缺页中断
    for (i = 0; i < MMAP_SIZE; i += PAGE_SIZE) {
        page_index = i / PAGE_SIZE;
        sprintf(buffer, "Page %d data written at offset %d", page_index, i);
        
        printf("测试程序: 写入第 %d 页起始位置...\n", page_index);
        strcpy(mapped_mem + i, buffer);
        
        usleep(500000);  // 延迟0.5秒
    }
    
    printf("\n测试程序: 所有页面写入完成\n\n");
    
    // 读取每个页面的开始位置
    printf("测试程序: 现在读取每个页面起始位置的数据\n");
    for (i = 0; i < MMAP_SIZE; i += PAGE_SIZE) {
        page_index = i / PAGE_SIZE;
        printf("测试程序: 第 %d 页内容: %s\n", page_index, mapped_mem + i);
        
        usleep(100000);  // 延迟0.1秒
    }
    
    // 写入随机位置，看是否会触发新的页面故障
    printf("\n测试程序: 现在写入随机位置\n");
    for (i = 0; i < 5; i++) {
        int random_offset = rand() % MMAP_SIZE;
        page_index = random_offset / PAGE_SIZE;
        sprintf(buffer, "Random data at offset %d, page %d", random_offset, page_index);
        
        printf("测试程序: 写入随机位置，偏移量: %d (第 %d 页)...\n", 
               random_offset, page_index);
        strcpy(mapped_mem + random_offset, buffer);
        
        // 延迟
        usleep(500000);
    }
    
    // 取消映射
    printf("\n测试程序: 取消内存映射\n");
    if (munmap(mapped_mem, MMAP_SIZE) == -1) {
        perror("取消映射失败");
    }
    
    // 关闭设备
    close(fd);
    printf("测试程序: 设备文件已关闭\n");
    
    return 0;
} 