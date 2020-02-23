#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>

static int botmem_probe(struct platform_device *pdev);
static int botmem_remove(struct platform_device *pdev);
static ssize_t botmem_read(struct file *file, char *buffer, size_t len, loff_t *offset);
static ssize_t botmem_write(struct file *file, const char *buffer, size_t len, loff_t *offset);

struct botmem_dev {
    struct miscdevice miscdev;
    void __iomem *regs;
    u8 mem_value;
};

static struct of_device_id botmem_dt_ids[] = {
    {
        .compatible = "dev,botmem"
    },
    { }
};

MODULE_DEVICE_TABLE(of, botmem_dt_ids);

static struct platform_driver botmem_platform = {
    .probe = botmem_probe,
    .remove = botmem_remove,
    .driver = {
        .name = "FOSSBot Memory Driver",
        .owner = THIS_MODULE,
        .of_match_table = botmem_dt_ids
    }
};

static const struct file_operations botmem_fops = {
    .owner = THIS_MODULE,
    .read = botmem_read,
    .write = botmem_write
};

