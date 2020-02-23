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

static int botmem_init(void) {
    int ret_val = 0;
    pr_info("Initializing FOSSBot Memory module\n");
    ret_val = platform_driver_register(&botmem_platform);
    if (ret_val != 0) {
        pr_err("platform_driver_register returned %d\n", ret_val);
        return ret_val;
    }
    pr_info("Custom LEDs module initialized\n");
    return 0;
}

static int botmem_probe(struct platform_device *pdev) {
    int ret_val = -EBUSY;
    struct botmem_dev *dev;
    struct resource *res = 0;

    pr_info("Probing FOSSBot Memory module\n");
    
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        pr_err("failed to get IORESOURCE_MEM\n");
        goto bad_exit;
    }

    dev = devm_kzalloc(&pdev->dev, sizeof(struct botmem_dev), GFP_KERNEL);
    dev->regs = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(dev->regs)) {
        goto bad_ioremap;
    }

    dev->mem_value = 0xFF;
    iowrite32(dev->mem_value, dev->regs);

    dev->miscdev.minor = MISC_DYNAMIC_MINOR;
    dev->miscdev.name = "botmem";
    dev->miscdev.fops = &botmem_fops;

    ret_val = misc_register(&dev->miscdev);
    if (ret_val) {
        pr_info("Failed to register misc device\n");
        goto bad_exit;
    }

    platform_set_drvdata(pdev, (void*)dev);
    pr_info("Finished probing FOSSBot Memory module\n");
    return 0;

bad_ioremap:
    ret_val = PTR_ERR(dev->regs);
bad_exit:
    pr_info("FOSSBot Memory module probe failed\n");
    return ret_val;
}

static ssize_t botmem_read(struct file *file, char *buffer, size_t len, loff_t *offset) {
    struct botmem_dev *dev = container_of(file->private_data, struct botmem_dev, miscdev);
    int success = copy_to_user(buffer, &dev->mem_value, sizeof(dev->mem_value));
    if (!success) {
        pr_info("Failed to read memory value\n");
        return -EFAULT;
    }

    return 0;
}

static ssize_t botmem_write(struct file *file, const char *buffer, size_t len, loff_t *offset) { 
    struct botmem_dev *dev = container_of(file->private_data, struct botmem_dev, miscdev);
    int success = copy_from_user(buffer, &dev->mem_value, sizeof(dev->mem_value));
    if (!success) {
        pr_info("Failed to write memory value\n");
        return -EFAULT;
    } else {
        iowrite32(dev->mem_value, dev->regs);
    }

    return len;
}

static int botmem_remove(struct platform_device *pdev) {
    struct botmem_dev *dev = (struct botmem_dev*)platform_get_drvdata(pdev);
    pr_info("Removing FOSSBot Memory module\n");

    iowrite32(0x00, dev->regs);
    misc_deregister(&dev->miscdev);
    pr_info("Removed FOSSBot Memory module\n");

    return 0;
}

static void botmem_exit(void) {
    pr_info("Unregistering FOSSBot Memory module\n");
    platform_driver_unregister(&botmem_platform);
    pr_info("Unregistered FOSSBot Memory module\n");
}

module_init(botmem_init);
module_exit(botmem_exit);

