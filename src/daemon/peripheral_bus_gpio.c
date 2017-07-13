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

#include "gpio.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
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
	int edge, direction;
	int ret;

	if (!peripheral_bus_gpio_is_available(pin, pb_data)) {
		_E("gpio %d is not available", pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = gpio_open(pin)) < 0) {
		_E("gpio_open error (%d)", ret);
		goto open_err;
	}

	if ((ret = gpio_get_edge_mode(pin, (gpio_edge_e*)&edge)) < 0) {
		_E("gpio_get_edge_mode error (%d)", ret);
		goto err;
	}

	if ((ret = gpio_get_direction(pin, (gpio_direction_e*)&direction)) < 0) {
		_E("gpio_get_direction error (%d)", ret);
		goto err;
	}

	gpio_handle = peripheral_bus_data_new(&pb_data->gpio_list);
	if (!gpio_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_UNKNOWN;
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
	gpio_handle->dev.gpio.edge = edge;
	gpio_handle->dev.gpio.direction = direction;
	gpio_handle->dev.gpio.gpio_skeleton = pb_data->gpio_skeleton;

	*handle = gpio_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	gpio_close(pin);

open_err:
	return ret;
}

int peripheral_bus_gpio_set_direction(pb_data_h handle, gint direction)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;

	gpio->direction = direction;

	if ((ret = gpio_set_direction(gpio->pin, (gpio_direction_e)direction)) < 0) {
		_E("gpio_set_direction error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_get_direction(pb_data_h handle, gint *direction)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;
	gint value;

	if ((ret = gpio_get_direction(gpio->pin, (gpio_direction_e*)direction)) < 0) {
		_E("gpio_get_direction error (%d)", ret);
		return ret;
	}

	if (*direction == GPIO_DIRECTION_OUT) {
		if ((ret = gpio_read(gpio->pin, &value)) < 0)
			return ret;
		/* Update direction state with the current value */
		*direction = GPIO_DIRECTION_OUT + value;
	}

	gpio->direction = *direction;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_set_edge(pb_data_h handle, gint edge)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;

	if ((ret = gpio_set_edge_mode(gpio->pin, (gpio_edge_e)edge)) < 0) {
		_E("gpio_set_edge_mode error (%d)", ret);
		return ret;
	}

	gpio->edge = edge;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_get_edge(pb_data_h handle, gint *edge)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;

	if ((ret = gpio_get_edge_mode(gpio->pin, (gpio_edge_e*)edge)) < 0) {
		_E("gpio_get_edge_mode error (%d)", ret);
		return ret;
	}

	gpio->edge = *edge;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_write(pb_data_h handle, gint value)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;

	/* Return error if direction is input mode */
	if (gpio->direction == GPIO_DIRECTION_IN)
		return PERIPHERAL_ERROR_IO_ERROR;

	if ((ret = gpio_write(gpio->pin, value)) < 0) {
		_E("gpio_write error (%d)", ret);
		return ret;
	}
	/* Update direction state along with the value */
	gpio->direction = GPIO_DIRECTION_OUT + ((value > 0) ? 1 : 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_read(pb_data_h handle, gint *value)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	int ret;

	if ((ret = gpio_read(gpio->pin, value)) < 0) {
		_E("gpio_read error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}

static gboolean  peripheral_bus_gpio_cb(GIOChannel *io, GIOCondition condition, gpointer data)
{
	peripheral_bus_gpio_s *gpio_data = (peripheral_bus_gpio_s *)data;
	struct timeval time;
	unsigned long long timestamp;
	GIOStatus status;
	gchar* strval;
	int value;

	if (!gpio_data->irq_en)
		return TRUE;

	if (gpio_data->direction != PERIPHERAL_GPIO_DIRECTION_IN
		|| gpio_data->edge == PERIPHERAL_GPIO_EDGE_NONE)
		return TRUE;

	gettimeofday(&time, NULL);

	g_io_channel_seek_position(io, 0, G_SEEK_SET, NULL);
	status = g_io_channel_read_line(io, &strval, NULL, NULL, NULL);
	if (status != G_IO_STATUS_NORMAL) {
		_E("Unable to read gpio value (%d)", status);
		return FALSE;
	}
	g_strstrip(strval);

	_D("gpio = %d, data = %s", gpio_data->pin, strval);

	if (g_ascii_strcasecmp(strval, "1") == 0)
		value = 1;
	else if (g_ascii_strcasecmp(strval, "0") == 0)
		value = 0;
	else {
		g_free(strval);
		_E("Error: gpio value is error");
		return FALSE;
	}
	g_free(strval);

	if (gpio_data->edge == PERIPHERAL_GPIO_EDGE_RISING && value == 0)
		return TRUE;
	else if (gpio_data->edge == PERIPHERAL_GPIO_EDGE_FALLING && value == 1)
		return TRUE;

	timestamp = (unsigned long long)time.tv_sec * 1000000 + time.tv_usec;
	peripheral_bus_emit_gpio_changed(gpio_data->gpio_skeleton, gpio_data->pin, value, timestamp);

	return TRUE;
}

int peripheral_bus_gpio_register_irq(pb_data_h handle)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;
	GIOStatus status;
	gchar* strval;

	if ((gpio->value_fd = gpio_open_isr(gpio->pin)) < 0)
		goto err_open_isr;

	gpio->io = g_io_channel_unix_new(gpio->value_fd);
	if (gpio->io == NULL) {
		_E("g_io_channel_unix_new error (%d)", gpio->value_fd);
		goto err_io_channel_new;
	}

	g_io_channel_seek_position(gpio->io, 0, G_SEEK_SET, NULL);
	status = g_io_channel_read_line(gpio->io, &strval, NULL, NULL, NULL);
	if (status != G_IO_STATUS_NORMAL) {
		_E("Unable to read gpio value (%d)", status);
		goto err_io_channel_read;
	}
	g_free(strval);

	gpio->io_id = g_io_add_watch(gpio->io, G_IO_PRI, peripheral_bus_gpio_cb, gpio);
	if (gpio->io_id == 0) {
		_E("g_io_add_watch error (%d)", gpio->io);
		goto err_io_add_watch;
	}
	g_io_channel_unref(gpio->io);
	gpio->irq_en = 1;

	return PERIPHERAL_ERROR_NONE;

err_io_add_watch:
err_io_channel_read:
	g_io_channel_unref(gpio->io);
	gpio->io_id = 0;
err_io_channel_new:
	gpio_close_isr(gpio->value_fd);
	gpio->io = 0;
err_open_isr:
	gpio->value_fd = 0;

	return PERIPHERAL_ERROR_UNKNOWN;
}

int peripheral_bus_gpio_unregister_irq(pb_data_h handle)
{
	peripheral_bus_gpio_s *gpio = &handle->dev.gpio;

	if (gpio->io) {
		gpio->irq_en = 0;
		g_source_remove(gpio->io_id);
		gpio->io_id = 0;
		g_io_channel_unref(gpio->io);
		gpio->io = 0;
		gpio_close_isr(gpio->value_fd);
		gpio->value_fd = 0;
	}

	return PERIPHERAL_ERROR_NONE;
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
