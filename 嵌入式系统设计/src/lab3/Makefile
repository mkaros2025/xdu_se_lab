obj-m := timer_driver.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install:
	insmod timer_driver.ko

uninstall:
	rmmod timer_driver

test:
	if lsmod | grep timer_driver > /dev/null; then \
		rmmod timer_driver; \
	fi
	insmod timer_driver.ko
	sleep 5
	dmesg | tail -30
	if lsmod | grep timer_driver > /dev/null; then \
		rmmod timer_driver; \
		dmesg | tail -10; \
	else \
		echo "模块已经被卸载"; \
	fi

.PHONY: default clean install uninstall test 