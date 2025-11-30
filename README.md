# lkm-invisible-proc

Linux kernel module to make your process becomes invisible, unkillable from the OS. For more detail about implementation visit [Main Blog](https://embeddedos.github.io/posts/kprobe-invisible-process/).

![Invisible process](https://github.com/EmbeddedOS/embeddedos.github.io/blob/main/assets/img/invisible_process.png)


## Setup

Build initrd image, make sure you build the busybox first, and then change the `BUSYBOX_PATH` point to your busybox address, after that:

```bash
./build_initrd.sh
```

Build kernel module and install into initfs:

```bash
make
make install
```

Run qemu (Change `LINUX_PATH` to your linux kernel source code):

```bash
./qemu.sh
```

Listen on the host machine: 

```bash
nc -u -l -p 3000
```
