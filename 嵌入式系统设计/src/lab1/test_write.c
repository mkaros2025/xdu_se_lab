#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/simple_char_dev"

int main()
{
    int fd;
    char buffer[] = "mk472";  // 固定数据
    ssize_t bytes_written;

    printf("写入程序：正在打开设备文件...\n");
    fd = open(DEVICE_PATH, O_WRONLY);
    if (fd < 0) {
        perror("无法打开设备文件");
        return -1;
    }
    printf("写入程序：设备文件已成功打开\n");

    // 确保从文件开始处写入
    lseek(fd, 0, SEEK_SET);

    // 写入固定数据到设备
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("写入失败");
        close(fd);
        return -1;
    }

    printf("写入程序：成功写入 %ld 字节数据\n", bytes_written);
    printf("写入的内容: %s\n", buffer);

    // 关闭设备
    close(fd);
    printf("写入程序：设备文件已关闭\n");

    return 0;
}