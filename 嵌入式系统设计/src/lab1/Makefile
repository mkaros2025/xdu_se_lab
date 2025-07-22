obj-m := simple_char_driver.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# 用户程序编译变量
USER_CC := gcc
USER_CFLAGS := -Wall -g

all: driver apps

driver:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

apps: test_read test_write

test_read: test_read.c
	$(USER_CC) $(USER_CFLAGS) -o $@ $<

test_write: test_write.c
	$(USER_CC) $(USER_CFLAGS) -o $@ $<

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f test_read test_write

install:
	# 加载驱动模块
	insmod simple_char_driver.ko
	# 创建设备节点
	mknod /dev/simple_char_dev c 240 0
	chmod 666 /dev/simple_char_dev

uninstall:
	# 移除设备节点
	rm -f /dev/simple_char_dev
	# 卸载驱动模块
	rmmod simple_char_driver

.PHONY: all driver apps clean install uninstall