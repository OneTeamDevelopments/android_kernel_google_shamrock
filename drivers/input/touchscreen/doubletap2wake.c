/*
 * drivers/input/touchscreen/doubletap2wake.c
 *
 *
 * Copyright (c) 2013 Dennis Rassmann <showp1984@gmail.com>
 * Copyright (c) 2013-19 Aaron Segaert <asegaert@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/input/doubletap2wake.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/input.h>
#include <linux/hrtimer.h>

#define DRIVER_AUTHOR "Caner Aydın <@caneray>"
#define DRIVER_DESCRIPTION "Doubletap2wake for shamrock device"
#define DRIVER_VERSION "1.1"

/* Tunables */
#define DT2W_DEFAULT		0

/* Resources */
int dt2w_switch = DT2W_DEFAULT;

/*
 * SYSFS stuff below here
 */
static ssize_t doubletap2wake_show(struct kobject *kobj, struct kobj_attribute *attr,
		      char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", dt2w_switch);
}

static ssize_t doubletap2wake_store(struct kobject *kobj, struct kobj_attribute *attr,
		       const char *buf, size_t count)
{
	int ret, val;
	
	ret = kstrtoint(buf, 0, &val);
	if (ret < 0)
		return ret;

	if (val < 0 || val > 1)
		val = 0;

	dt2w_switch = val;

	return count;
}

static struct kobj_attribute doubletap2wake_attribute =
	__ATTR(doubletap2wake, 0664, doubletap2wake_show, doubletap2wake_store);

static struct attribute *attrs[] = {
	&doubletap2wake_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

struct kobject *android_touch_kobj;
EXPORT_SYMBOL_GPL(android_touch_kobj);

/*
 * INIT / EXIT stuff below here
 */

static int __init wake_gestures_init(void)
{
	int rc = 0;

	pr_info("start wake gestures\n");

	android_touch_kobj = kobject_create_and_add("android_touch", NULL);
	if (!android_touch_kobj) {
		pr_info("fail!!!!\n");
		return -ENOMEM;
	}
	rc = sysfs_create_group(android_touch_kobj, &attr_group);
	if (rc)
		pr_warn("%s: sysfs_create_group failed\n", __func__);

	return 0;
}

static void __exit wake_gestures_exit(void)
{
	kobject_del(android_touch_kobj);

	return;
}

module_init(wake_gestures_init);
module_exit(wake_gestures_exit);

