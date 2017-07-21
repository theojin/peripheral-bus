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
#include "peripheral_bus_i2c.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"
#include "peripheral_bus_gdbus_i2c.h"

static void __i2c_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_data_h i2c_handle = (pb_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(i2c_handle->watch_id);
	peripheral_bus_i2c_close(i2c_handle);
}

gboolean handle_i2c_open(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint bus,
		gint address,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h i2c_handle;

	if ((ret = peripheral_bus_i2c_open(bus, address, &i2c_handle, user_data)) < PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &i2c_handle->client_info) < 0) {
		peripheral_bus_i2c_close(i2c_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}

	i2c_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			i2c_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__i2c_on_name_vanished,
			i2c_handle,
			NULL);
	_D("bus : %d, address : %d, id = %s", bus, address, i2c_handle->client_info.id);

out:
	peripheral_io_gdbus_i2c_complete_open(i2c, invocation, GPOINTER_TO_UINT(i2c_handle), ret);

	return true;
}

gboolean handle_i2c_close(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h i2c_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, i2c_handle, pb_data->i2c_list) != 0) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else {
		g_bus_unwatch_name(i2c_handle->watch_id);
		ret = peripheral_bus_i2c_close(i2c_handle);
	}

	peripheral_io_gdbus_i2c_complete_close(i2c, invocation, ret);

	return true;
}

gboolean handle_i2c_read(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h i2c_handle = GUINT_TO_POINTER(handle);
	GVariant *data_array = NULL;
	uint8_t err_buf[2] = {0, };

	if (peripheral_bus_handle_is_valid(invocation, i2c_handle, pb_data->i2c_list) != 0) {
		_E("i2c handle is not valid");
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_i2c_read(i2c_handle, length, &data_array);

	peripheral_io_gdbus_i2c_complete_read(i2c, invocation, data_array, ret);

	return true;
}

gboolean handle_i2c_write(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h i2c_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, i2c_handle, pb_data->i2c_list) != 0) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_i2c_write(i2c_handle, length, data_array);

	peripheral_io_gdbus_i2c_complete_write(i2c, invocation, ret);

	return true;
}

gboolean handle_i2c_smbus_ioctl(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar read_write,
		guchar command,
		guint size,
		guint16 data_in,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h i2c_handle = GUINT_TO_POINTER(handle);
	uint16_t data = 0xFFFF;

	if (peripheral_bus_handle_is_valid(invocation, i2c_handle, pb_data->i2c_list) != 0) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_i2c_smbus_ioctl(i2c_handle, read_write, command, size, data_in, &data);

	peripheral_io_gdbus_i2c_complete_smbus_ioctl(i2c, invocation, data, ret);

	return true;
}
