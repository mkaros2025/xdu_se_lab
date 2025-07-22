# 简单字符设备驱动实验

本实验实现了一个简单的字符设备驱动程序，用于进程间通信。

## 文件结构

- `simple_char_driver.c`: 字符设备驱动程序
- `test_read.c`: 读取测试程序
- `test_write.c`: 写入测试程序
- `Makefile`: 用于编译和安装

## 编译方法

编译驱动程序和测试程序：

```
make
```

## 安装和卸载

安装驱动程序（需要root权限）：

```
sudo make install
```

卸载驱动程序（需要root权限）：

```
sudo make uninstall
```

## 测试方法

1. 运行写入程序：

```
./test_write
```

2. 在另一个终端运行读取程序：

```
./test_read
```

## 查看内核日志

可以通过以下命令查看驱动程序的内核日志：

```
sudo dmesg | grep simple_char_driver
``` 