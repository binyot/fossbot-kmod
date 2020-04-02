# fossbot-kmod

Kernel module(s) for the fossbot project.

## botmem

Creates a character device `/dev/botmem` to communicate with onboard Avalon Memory Mapped Slave. To build, modify Makefile to match your kernel source dir and run `make`. To use, load built module(s) with your preferred method.

Device tree should contain the botmem device for this module to work.

### Example usage

```
$ make
# insmod botmem.ko
$ echo -n -e '\x00' > /dev/botmem
$ echo -n -e '\xAB' > /dev/botmem
# rmmod botmem
```
