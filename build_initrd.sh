#!/bin/bash
# Original structure preserved
#BUSYBOX_PATH=/home/conguyen/repos/personal/busybox-1.36.1
BUSYBOX_PATH=/home/yacovs/qemu_kernel/kprobe-invisible-process/Busybox-static/busybox_arm64
CURRENT_DIR=$(pwd)

rm -rf rootfs.cpio.gz
rm -rf rootfs
mkdir -p rootfs/{bin,sbin,etc,proc,sys,usr/{bin,sbin},dev,lib,var/{log,run}}
cd rootfs

# Create minimal device nodes
sudo mknod -m 660 dev/mem c 1 1
sudo mknod -m 660 dev/tty2 c 4 2
sudo mknod -m 660 dev/tty3 c 4 3
sudo mknod -m 660 dev/tty4 c 4 4
sudo mknod -m 660 dev/null c 1 3
sudo mknod -m 660 dev/zero c 1 5
sudo mknod -m 600 dev/console c 5 1 

# Copy precompiled BusyBox binary
cp ${BUSYBOX_PATH} bin/busybox
chmod +x bin/busybox

# Use BusyBox as init
ln -sf bin/busybox init

# Create symlinks for common commands
cd bin
for cmd in grep rmmod lsmod modinfo sh ls cat echo mount ps ifconfig ip route sleep dmesg insmod modprobe; do
    ln -sf busybox $cmd
done
#cd "$CURRENT_DIR"
cd ../
# Create init.d scripts
mkdir -p etc/init.d/
cat > etc/init.d/rcS << EOF0
# Mounting system...
mount -t sysfs none /sys
mount -t proc none /proc

# Configure networking for QEMU...
ifconfig lo up
ifconfig eth0 up
ip a add 10.0.2.15/24 dev eth0
route add default gw 10.0.2.2 eth0

cat > /etc/resolv.conf << EOF1
nameserver 10.0.2.3
EOF1
EOF0

chmod -R 777 etc/init.d/rcS

# Build initramfs
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../rootfs.cpio.gz
cd -

