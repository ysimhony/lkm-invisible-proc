#!/bin/bash

LINUX_PATH=/home/conguyen/repos/personal/linux
KERNEL_IMAGE=${LINUX_PATH}/arch/arm64/boot/Image
INITRD_IMAGE_PATH=$(pwd)/rootfs.cpio.gz

qemu-system-aarch64 -machine virt -cpu cortex-a57 -smp 4 -m 2G -kernel ${KERNEL_IMAGE}  -nographic -initrd ${INITRD_IMAGE_PATH}  -netdev user,id=net0,hostfwd=udp::3000-:3000 -device virtio-net-device,netdev=net0
