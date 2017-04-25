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
#include <peripheral_internal.h>

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

	return PERIPHERAL_ERROR_NONE;

err:
	gpio_close(gpio->pin);

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

	gpio->direction = *direction;

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

	if ((ret = gpio_write(pin, value)) < 0) {
		_E("gpio_write error (%d)", ret);
		return ret;
	}

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
