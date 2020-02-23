#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>

struct botmem_dev {
    struct miscdevice miscdev;
    void __iomem *regs;
    u8 mem_value;
};

