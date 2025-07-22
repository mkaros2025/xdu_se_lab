# 简单Proc文件系统驱动实验

本实验实现了一个简单的proc文件系统驱动程序，用于读写内核缓冲区。

## 实验要求

1. 定义整型模块参数N，用于指定缓冲区大小
2. 定义全局char指针buffer_ptr
3. 在加载函数中使用kmalloc申请大小为N的空间
4. 在卸载函数中使用kfree释放该空间
5. 通过proc文件系统对该buffer进行读写操作

## 文件结构

- `simple_proc_driver.c`: proc驱动程序
- `Makefile`: 用于编译和安装

## 编译方法

编译驱动程序：

```
make
```

## 安装和卸载

安装驱动程序（需要root权限）：

```
sudo make install
```

或者手动指定缓冲区大小：

```
sudo insmod simple_proc_driver.ko buffer_size=4096
```

卸载驱动程序（需要root权限）：

```
sudo make uninstall
```

## 测试方法

1. 写入数据到proc文件：

```
echo "测试数据" > /proc/buffer_proc
```

或者使用预设测试：

```
sudo make test_write
```

2. 读取proc文件中的数据：

```
cat /proc/buffer_proc
```

或者使用预设测试：

```
sudo make test_read
```

## 查看内核日志

可以通过以下命令查看驱动程序的内核日志：

```
sudo dmesg | grep simple_proc_driver
``` 