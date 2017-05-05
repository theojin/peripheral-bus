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

#include <peripheral_io.h>

#include "gpio.h"
#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"

static pb_gpio_data_h peripheral_bus_gpio_data_get(int pin, GList **list)
{
	GList *gpio_list = *list;
	GList *link;
	pb_gpio_data_h gpio_data = NULL;

	if (!gpio_list)
		return NULL;

	link = gpio_list;
	while (link) {
		gpio_data = (pb_gpio_data_h)link->data;
		if (gpio_data->pin == pin)
			return gpio_data;

		link = g_list_next(link);
	}

	return NULL;
}

static pb_gpio_data_h peripheral_bus_gpio_data_new(int pin, GList **list)
{
	GList *gpio_list = *list;
	pb_gpio_data_h gpio_data;

	gpio_data = (pb_gpio_data_h)calloc(1, sizeof(peripheral_bus_gpio_data_s));
	if (gpio_data == NULL) {
		_E("failed to allocate peripheral_bus_gpio_data_s");
		return NULL;
	}

	*list = g_list_append(gpio_list, gpio_data);

	gpio_data->pin = pin;

	return gpio_data;
}

static void peripheral_bus_gpio_data_free(int pin, GList **list)
{
	GList *gpio_list = *list;
	GList *link;
	pb_gpio_data_h gpio_data;

	if (gpio_list == NULL)
		return;

	link = gpio_list;
	while (link) {
		gpio_data = (pb_gpio_data_h)link->data;

		if (gpio_data->pin == pin) {
			*list = g_list_remove_link(gpio_list, link);
			free(link->data);
			g_list_free(link);
			return;
		}
		link = g_list_next(link);
	}
}

int peripheral_bus_gpio_open(gint pin, gint *edge, gint *direction, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (gpio) {
		_E("gpio %d is busy", pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = gpio_open(pin)) < 0) {
		_E("gpio_open error (%d)", ret);
		goto open_err;
	}

	if ((ret = gpio_get_edge_mode(pin, (gpio_edge_e*)edge))  < 0) {
		_E("gpio_get_edge_mode error (%d)", ret);
		goto err;
	}

	if ((ret = gpio_get_direction(pin, (gpio_direction_e*)direction)) < 0) {
		_E("gpio_get_direction error (%d)", ret);
		goto err;
	}

	gpio = peripheral_bus_gpio_data_new(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_new error");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto err;
	}

	gpio->pin = pin;
	gpio->edge = *edge;
	gpio->direction = *direction;
	gpio->gpio_skeleton = pb_data->gpio_skeleton;

	return PERIPHERAL_ERROR_NONE;

err:
	gpio_close(pin);

open_err:
	return ret;
}

int peripheral_bus_gpio_set_direction(gint pin, gint direction, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	gpio->direction = direction;

	if ((ret = gpio_set_direction(pin, (gpio_direction_e)direction)) < 0) {
		_E("gpio_set_direction error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_get_direction(gint pin, gint *direction, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	gint value;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	if ((ret = gpio_get_direction(pin, (gpio_direction_e*)direction)) < 0) {
		_E("gpio_get_direction error (%d)", ret);
		return ret;
	}

	if (*direction == GPIO_DIRECTION_OUT) {
		if ((ret = gpio_read(pin, &value)) < 0)
			return ret;
		/* Update direction state with the current value */
		*direction = GPIO_DIRECTION_OUT + value;
		gpio->direction = *direction;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_set_edge(gint pin, gint edge, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	gpio->edge = edge;

	if ((ret = gpio_set_edge_mode(pin, (gpio_edge_e)edge)) < 0) {
		_E("gpio_set_edge_mode error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_get_edge(gint pin, gint *edge, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	if ((ret = gpio_get_edge_mode(pin, (gpio_edge_e*)edge)) < 0) {
		_E("gpio_get_edge_mode error (%d)", ret);
		return ret;
	}

	gpio->edge = *edge;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_write(gint pin, gint value, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	/* Return error if direction is input mode */
	if (gpio->direction == GPIO_DIRECTION_IN)
		return PERIPHERAL_ERROR_IO_ERROR;

	if ((ret = gpio_write(pin, value)) < 0) {
		_E("gpio_write error (%d)", ret);
		return ret;
	}
	/* Update direction state along with the value */
	gpio->direction = GPIO_DIRECTION_OUT + (value > 0) ? 1 : 0;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_read(gint pin, gint *value, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	int ret;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	if ((ret = gpio_read(pin, value)) < 0) {
		_E("gpio_read error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}

static gboolean  peripheral_bus_gpio_cb(GIOChannel *io, GIOCondition condition, gpointer data)
{
	pb_gpio_data_h gpio_data = (pb_gpio_data_h)data;
	GIOStatus status;
	gchar* strval;
	int value;

	if (!gpio_data->irq_en)
		return TRUE;

	if (gpio_data->direction != PERIPHERAL_GPIO_DIRECTION_IN
		|| gpio_data->edge == PERIPHERAL_GPIO_EDGE_NONE)
		return TRUE;

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

	peripheral_bus_emit_gpio_changed(gpio_data->gpio_skeleton, gpio_data->pin, value);

	return TRUE;
}

int peripheral_bus_gpio_register_irq(gint pin, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;
	GIOStatus status;
	gchar* strval;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	if ((gpio->value_fd = gpio_open_isr(pin)) < 0)
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

int peripheral_bus_gpio_unregister_irq(gint pin, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio;

	gpio = peripheral_bus_gpio_data_get(pin, &pb_data->gpio_list);
	if (!gpio) {
		_E("peripheral_bus_gpio_data_get error");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

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

int peripheral_bus_gpio_close(gint pin, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	int ret;

	peripheral_bus_gpio_data_free(pin, &pb_data->gpio_list);

	if ((ret = gpio_close(pin)) < 0) {
		_E("gpio_close error (%d)", ret);
		return ret;
	}

	return PERIPHERAL_ERROR_NONE;
}
