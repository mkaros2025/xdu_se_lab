#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/simple_char_dev"
#define BUFFER_SIZE 1024

int main()
{
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("读取程序：正在打开设备文件...\n");
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("无法打开设备文件");
        return -1;
    }
    printf("读取程序：设备文件已成功打开\n");

    // 确保从文件开始处读取
    lseek(fd, 0, SEEK_SET);

    // 从设备读取数据
    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("读取失败");
        close(fd);
        return -1;
    }

    // 添加字符串结束符
    buffer[bytes_read] = '\0';

    printf("读取程序：成功读取 %ld 字节数据\n", bytes_read);
    printf("读取的内容: %s\n", buffer);

    // 关闭设备
    close(fd);
    printf("读取程序：设备文件已关闭\n");

    return 0;
}