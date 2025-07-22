obj-m := simple_proc_driver.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	insmod simple_proc_driver.ko buffer_size=2048

uninstall:
	rmmod simple_proc_driver

test_read:
	cat /proc/buffer_proc

test_write:
	echo "Hello from mk472" > /proc/buffer_proc

.PHONY: default clean install uninstall test_read test_write 