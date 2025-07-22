obj-m := mmap_driver.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

USWR_CC := gcc
USER_CFLAGS := -Wall -g

all: driver app

driver:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

app: test_mmap

test_mmap: test_mmap.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f test_mmap

install:
	if lsmod | grep mmap_driver > /dev/null; then \
		rmmod mmap_driver; \
	fi
	insmod mmap_driver.ko
	# 创建设备文件接入点
	mknod /dev/mmap_device c 241 0
	chmod 666 /dev/mmap_device

uninstall:
	rm -f /dev/mmap_device
	rmmod mmap_driver

test: app
	if lsmod | grep mmap_driver > /dev/null; then \
		rmmod mmap_driver; \
	fi
	rm -f /dev/mmap_device
	insmod mmap_driver.ko
	mknod /dev/mmap_device c 241 0
	chmod 666 /dev/mmap_device
	./test_mmap
	dmesg | tail -20

.PHONY: all driver app clean install uninstall test 