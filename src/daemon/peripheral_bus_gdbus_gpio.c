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

#include <peripheral_io.h>

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_bus_gpio.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"
#include "peripheral_bus_gdbus_gpio.h"

static void __gpio_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_data_h gpio_handle = (pb_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(gpio_handle->watch_id);
	peripheral_bus_gpio_close(gpio_handle);
}

gboolean handle_gpio_open(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h gpio_handle = NULL;

	if ((ret = peripheral_bus_gpio_open(pin, &gpio_handle, user_data)) < PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &gpio_handle->client_info) < 0) {
		peripheral_bus_gpio_close(gpio_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		gpio_handle = NULL;
		goto out;
	}

	gpio_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			gpio_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__gpio_on_name_vanished,
			gpio_handle,
			NULL);
	_D("gpio : %d, id = %s", gpio_handle->dev.gpio.pin, gpio_handle->client_info.id);

out:
	peripheral_io_gdbus_gpio_complete_open(gpio, invocation, GPOINTER_TO_UINT(gpio_handle), ret);

	return true;
}

gboolean handle_gpio_close(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else {
		g_bus_unwatch_name(gpio_handle->watch_id);
		ret = peripheral_bus_gpio_close(gpio_handle);
	}

	peripheral_io_gdbus_gpio_complete_close(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_set_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint direction,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_set_direction(gpio_handle, direction);

	peripheral_io_gdbus_gpio_complete_set_direction(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_set_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint edge,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_set_edge(gpio_handle, edge);

	peripheral_io_gdbus_gpio_complete_set_edge_mode(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_set_interrupted_cb(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_set_interrupted_cb(gpio_handle);

	peripheral_io_gdbus_gpio_complete_set_interrupted_cb(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_unset_interrupted_cb(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_unset_interrupted_cb(gpio_handle);

	peripheral_io_gdbus_gpio_complete_unset_interrupted_cb(gpio, invocation, ret);

	return true;
}

void peripheral_bus_emit_interrupted_cb(
		PeripheralIoGdbusGpio *gpio,
		gint pin,
		gint value,
		guint64 timestamp)
{
	g_assert(gpio != NULL);

	peripheral_io_gdbus_gpio_emit_interrupted_cb(gpio, pin, value, timestamp);
}

gboolean handle_gpio_read(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);
	gint read_value = 0;

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_read(gpio_handle, &read_value);

	peripheral_io_gdbus_gpio_complete_read(gpio, invocation, read_value, ret);

	return true;
}

gboolean handle_gpio_write(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint value,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h gpio_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, gpio_handle, pb_data->gpio_list) != 0) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_gpio_write(gpio_handle, value);

	peripheral_io_gdbus_gpio_complete_write(gpio, invocation, ret);

	return true;
}
