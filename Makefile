obj-m := lkm.o
KERNEL_DIR := /home/conguyen/repos/personal/linux
CROSS_COMPILE := /home/conguyen/repos/personal/arm64-toolchain/bin/aarch64-none-linux-gnu-
GCC := $(CROSS_COMPILE)gcc
ROOTFS_PATH := $(PWD)/rootfs
ARCH := arm64

all: usr
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -rf invisible_proc

usr:
	$(GCC) -o invisible_proc invisible_proc.c -static

install:
	cp $(PWD)/invisible_proc $(ROOTFS_PATH)/
	cp $(PWD)/lkm.ko $(ROOTFS_PATH)/
	cd $(ROOTFS_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > $(PWD)/rootfs.cpio.gz && cd -
