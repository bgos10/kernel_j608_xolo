/* drivers/misc/blx.c
*
* Copyright 2011 Ezekeel
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*/

#include <linux/init.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/blx.h>

#define BATTERYLIFEEXTENDER_VERSION 1

static int charging_limit = MAX_CHARGINGLIMIT;

static ssize_t blx_charginglimit_read(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", charging_limit);
}

static ssize_t blx_charginglimit_write(struct device * dev, struct device_attribute * attr, const char * buf, size_t size)
{
    unsigned int data;

    if(sscanf(buf, "%u\n", &data) == 1)
{
if (data >= 0 && data <= MAX_CHARGINGLIMIT)
{
charging_limit = data;

pr_info("BLX charging limit set to %u\n", charging_limit);
}
else
{
pr_info("%s: invalid input range %u\n", __FUNCTION__, data);
}
}
    else
{
pr_info("%s: invalid input\n", __FUNCTION__);
}

    return size;
}

static ssize_t blx_version(struct device * dev, struct device_attribute * attr, char * buf)
{
    return sprintf(buf, "%u\n", BATTERYLIFEEXTENDER_VERSION);
}

static DEVICE_ATTR(charging_limit, S_IRUGO | S_IWUGO, blx_charginglimit_read, blx_charginglimit_write);
static DEVICE_ATTR(version, S_IRUGO , blx_version, NULL);

static struct attribute *blx_attributes[] =
    {
&dev_attr_charging_limit.attr,
&dev_attr_version.attr,
NULL
    };

static struct attribute_group blx_group =
    {
.attrs = blx_attributes,
    };

static struct miscdevice blx_device =
    {
.minor = MISC_DYNAMIC_MINOR,
.name = "batterylifeextender",
    };

int get_charginglimit(void)
{
    return charging_limit;
}
EXPORT_SYMBOL(get_charginglimit);

static int __init blx_init(void)
{
    int ret;

    pr_info("%s misc_register(%s)\n", __FUNCTION__, blx_device.name);

    ret = misc_register(&blx_device);

    if (ret)
{
pr_err("%s misc_register(%s) fail\n", __FUNCTION__, blx_device.name);

return 1;
}

    if (sysfs_create_group(&blx_device.this_device->kobj, &blx_group) < 0)
{
pr_err("%s sysfs_create_group fail\n", __FUNCTION__);
pr_err("Failed to create sysfs group for device (%s)!\n", blx_device.name);
}

    return 0;
}

device_initcall(blx_init);
