/* SPDX-License-Identifier: GPL-2.0 */
/*
 * fl2000_registers.c
 *
 * (C) Copyright 2017, Fresco Logic, Incorporated.
 * (C) Copyright 2018-2019, Artem Mygaiev
 */

#include "fl2000.h"

#define CONTROL_MSG_LEN		4
#define CONTROL_MSG_READ	64
#define CONTROL_MSG_WRITE	65

struct fl2000_reg_data {
	struct usb_device *usb_dev;
#if defined(CONFIG_DEBUG_FS)
	unsigned int reg_debug_address;
#endif
};

static bool fl2000_reg_precious(struct device *dev, unsigned int reg)
{
	return FL2000_REG_PRECIOUS(reg);
}

static bool fl2000_reg_volatile(struct device *dev, unsigned int reg)
{
	return FL2000_REG_VOLATILE(reg);
}

static int fl2000_reg_read(void *context, unsigned int reg, unsigned int *val)
{
	int ret;
	struct fl2000_reg_data *reg_data = context;
	struct usb_device *usb_dev = reg_data->usb_dev;
	u16 offset = (u16)reg;
	u32 *data = kmalloc(sizeof(*data), GFP_KERNEL | GFP_DMA);

	BUG_ON(!data);

	ret = usb_control_msg(usb_dev, usb_rcvctrlpipe(usb_dev, 0),
			CONTROL_MSG_READ, (USB_DIR_IN | USB_TYPE_VENDOR), 0,
			offset, data, CONTROL_MSG_LEN, USB_CTRL_GET_TIMEOUT);
	if (ret > 0) {
		if (ret != CONTROL_MSG_LEN)
			ret = -1;
		else
			ret = 0;
	}

	*val = *data;

	dev_dbg(&usb_dev->dev, "RD: 0x%04X - 0x%08X", reg, *data);

	kfree(data);
	return ret;
}

static int fl2000_reg_write(void *context, unsigned int reg, unsigned int val)
{
	int ret;
	struct fl2000_reg_data *reg_data = context;
	struct usb_device *usb_dev = reg_data->usb_dev;
	u16 offset = (u16)reg;
	u32 *data = kmalloc(sizeof(*data), GFP_KERNEL | GFP_DMA);

	BUG_ON(!data);

	*data = val;

	dev_dbg(&usb_dev->dev, "WR: 0x%04X - 0x%08X", reg, *data);

	ret = usb_control_msg(usb_dev, usb_sndctrlpipe(usb_dev, 0),
			CONTROL_MSG_WRITE, (USB_DIR_OUT | USB_TYPE_VENDOR), 0,
			offset, data, CONTROL_MSG_LEN, USB_CTRL_SET_TIMEOUT);
	if (ret > 0) {
		if (ret != CONTROL_MSG_LEN)
			ret = -1;
		else
			ret = 0;
	}

	kfree(data);
	return ret;
}

static const struct regmap_config fl2000_regmap_config = {
	.val_bits = 32,
	.reg_bits = 16,
	.reg_stride = 4,
	.max_register = 0xFFFF,

	.cache_type = REGCACHE_NONE,

	.precious_reg = fl2000_reg_precious,
	.volatile_reg = fl2000_reg_volatile,

	.reg_defaults = fl2000_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(fl2000_reg_defaults),

	.reg_format_endian = REGMAP_ENDIAN_BIG,
	.val_format_endian = REGMAP_ENDIAN_BIG,

	.reg_read = fl2000_reg_read,
	.reg_write = fl2000_reg_write,

	.use_single_read = true,
	.use_single_write = true,
};

#if defined(CONFIG_DEBUG_FS)

static int fl2000_debugfs_reg_read(void *data, u64 *value)
{
	int ret;
	unsigned int int_value;
	struct fl2000_reg_data *reg_data = data;
	struct usb_device *usb_dev = reg_data->usb_dev;

	ret = fl2000_reg_read(usb_dev, reg_data->reg_debug_address, &int_value);
	*value = int_value;

	return ret;
}

static int fl2000_debugfs_reg_write(void *data, u64 value)
{
	struct fl2000_reg_data *reg_data = data;
	struct usb_device *usb_dev = reg_data->usb_dev;

	return fl2000_reg_write(usb_dev, reg_data->reg_debug_address, value);
}

DEFINE_SIMPLE_ATTRIBUTE(reg_ops, fl2000_debugfs_reg_read,
		fl2000_debugfs_reg_write, "%08llx\n");

static void fl2000_debugfs_reg_init(struct fl2000_reg_data *reg_data)
{
	struct dentry *root_dir;
	struct dentry *reg_address_file, *reg_data_file;
	struct usb_device *usb_dev = reg_data->usb_dev;

	root_dir = debugfs_create_dir("fl2000_regs", NULL);

	reg_address_file = debugfs_create_x32("reg_address", fl2000_debug_umode,
			root_dir, &reg_data->reg_debug_address);

	reg_data_file = debugfs_create_file("reg_data", fl2000_debug_umode,
			root_dir, usb_dev, &reg_ops);
}

#else /* CONFIG_DEBUG_FS */

#define fl2000_debugfs_reg_init(usb_dev)

#endif /* CONFIG_DEBUG_FS */

int fl2000_regmap_create(struct usb_device *usb_dev)
{
	struct fl2000_reg_data *reg_data;
	struct regmap *regmap;

	reg_data = devm_kzalloc(&usb_dev->dev, sizeof(*reg_data), GFP_KERNEL);
	if (!reg_data) {
		dev_err(&usb_dev->dev, "Registers data allocation failed");
		return -ENOMEM;
	}

	reg_data->usb_dev = usb_dev;

	regmap = devm_regmap_init(&usb_dev->dev, NULL, reg_data,
			&fl2000_regmap_config);
	if (IS_ERR(regmap)) {
		dev_err(&usb_dev->dev, "Registers map failed (%ld)",
				PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	fl2000_debugfs_reg_init(reg_data);

	dev_info(&usb_dev->dev, "Configured FL2000 registers");
	return 0;
}
