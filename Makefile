obj-m := vm.o
KERNEL_DIR := /home/conguyen/repos/personal/linux
CROSS_COMPILE := /home/conguyen/repos/personal/arm64-toolchain/bin/aarch64-none-linux-gnu-
ROOTFS_PATH := $(PWD)/rootfs
ARCH := arm64

all:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

install:
	cp $(PWD)/vm.ko $(ROOTFS_PATH)/
	cd $(ROOTFS_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > $(PWD)/rootfs.cpio.gz && cd -
