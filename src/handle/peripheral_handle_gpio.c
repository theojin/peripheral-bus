/*
 * Copyright (c) 2016-2017 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>
#include <poll.h>
#include <sys/time.h>
#include <libudev.h>

#include <peripheral_io.h>

#include "peripheral_interface_gpio.h"
#include "peripheral_bus.h"
#include "peripheral_log.h"
#include "peripheral_bus_util.h"

static bool peripheral_bus_gpio_is_available(int pin, peripheral_bus_s *pb_data)
{
	pb_board_dev_s *gpio = NULL;
	pb_data_h handle;
	GList *link;

	RETV_IF(pb_data == NULL, false);
	RETV_IF(pb_data->board == NULL, false);

	gpio = peripheral_bus_board_find_device(PB_BOARD_DEV_GPIO, pb_data->board, pin);
	if (gpio == NULL) {
		_E("Not supported GPIO pin : %d", pin);
		return false;
	}

	link = pb_data->gpio_list;
	while (link) {
		handle = (pb_data_h)link->data;
		if (handle->dev.gpio.pin == pin) {
			_E("gpio %d is busy", pin);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

static int gpio_wait_for_udev(int gpiopin)
{
	#define GPIO_NAME_LEN 8
	struct udev *udev;
	struct udev_monitor *monitor;
	struct udev_device *dev;
	struct pollfd pfd;
	char gpio_name[GPIO_NAME_LEN];
	int ret = -EIO;

	udev = udev_new();
	if (!udev) {
		_E("Cannot create udev");
		return ret;
	}

	monitor = udev_monitor_new_from_netlink(udev, "udev");
	if (!monitor) {
		_E("Cannot create udev monitor");
		udev_unref(udev);
		return ret;
	}

	ret = udev_monitor_filter_add_match_subsystem_devtype(monitor, "gpio", NULL);
	if (ret < 0) {
		_E("Failed to add monitor filter");
		goto out;
	}

	ret = udev_monitor_enable_receiving(monitor);
	if (ret < 0) {
		_E("Failed to enable udev receiving");
		goto out;
	}

	pfd.fd = udev_monitor_get_fd(monitor);
	pfd.events = POLLIN;

	snprintf(gpio_name, GPIO_NAME_LEN, "gpio%d", gpiopin);

	for (int cnt = 0; cnt < 10; cnt++) {
		if (poll(&pfd, 1, 100) < 0) {
			_E("Failed to watch udev monitor");
			goto out;
		}

		dev = udev_monitor_receive_device(monitor);
		if (dev) {
			if (strcmp(udev_device_get_sysname(dev), gpio_name) == 0) {
				_D("udev for %s is initialized", gpio_name);
				ret = 0;
				goto out;
			}
		}
	}
	_E("Time out");

out:
	udev_monitor_unref(monitor);
	udev_unref(udev);

	return ret;
}


int peripheral_bus_gpio_open(gint pin, pb_data_h *handle, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h gpio_handle;
	int ret;

	if (!peripheral_bus_gpio_is_available(pin, pb_data)) {
		_E("gpio %d is not available", pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = gpio_open(pin)) < 0) {
		_E("gpio_open error (%d)", ret);
		goto open_err;
	}

	gpio_handle = peripheral_bus_data_new(&pb_data->gpio_list);
	if (!gpio_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	if (gpio_wait_for_udev(pin) < 0) {
		_E("device nodes are not writable");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto err;
	}

	gpio_handle->type = PERIPHERAL_BUS_TYPE_GPIO;
	gpio_handle->list = &pb_data->gpio_list;
	gpio_handle->dev.gpio.pin = pin;

	*handle = gpio_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	gpio_close(pin);

open_err:
	return ret;
}

int peripheral_bus_gpio_close(pb_data_h handle)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret = PERIPHERAL_ERROR_NONE;

	if ((ret = gpio_close(gpio->pin)) < 0)
		return ret;

	if (peripheral_bus_data_free(handle) < 0) {
		_E("Failed to free gpio data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;;
}
