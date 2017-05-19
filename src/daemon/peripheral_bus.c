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
#include <string.h>
#include <gio/gio.h>

#include <peripheral_io.h>
#include <peripheral_gdbus.h>

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_bus_gpio.h"
#include "peripheral_bus_i2c.h"
#include "peripheral_bus_pwm.h"
#include "peripheral_bus_uart.h"
#include "peripheral_common.h"

static int peripheral_bus_get_client_info(GDBusMethodInvocation *invocation, peripheral_bus_s *pb_data, pb_client_info_s *client_info)
{
	guint pid = 0;
	GError *error = NULL;
	GVariant *_ret;
	const gchar *id;

	id = g_dbus_method_invocation_get_sender(invocation);
	_ret = g_dbus_connection_call_sync(pb_data->connection,
		"org.freedesktop.DBus",
		"/org/freedesktop/DBus",
		"org.freedesktop.DBus",
		"GetConnectionUnixProcessID",
		g_variant_new("(s)", id),
		NULL,
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		&error);

	if (_ret == NULL) {
		_E("Failed to get client pid, %s", error->message);
		g_error_free(error);

		return -1;
	}

	g_variant_get(_ret, "(u)", &pid);
	g_variant_unref(_ret);

	client_info->pid = (pid_t)pid;
	client_info->pgid = getpgid(pid);
	client_info->id = strdup(id);

	return 0;
}

gboolean handle_gpio_open(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	gint edge;
	gint direction;

	ret = peripheral_bus_gpio_open(pin, &edge, &direction, user_data);
	peripheral_io_gdbus_gpio_complete_open(gpio, invocation, edge, direction, ret);

	return true;
}

gboolean handle_gpio_close(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_close(pin, user_data);
	peripheral_io_gdbus_gpio_complete_close(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_get_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	gint direction;

	ret = peripheral_bus_gpio_get_direction(pin, &direction, user_data);
	peripheral_io_gdbus_gpio_complete_get_direction(gpio, invocation, direction, ret);

	return true;
}

gboolean handle_gpio_set_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gint direction,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_set_direction(pin, direction, user_data);
	peripheral_io_gdbus_gpio_complete_set_direction(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_read(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	gint read_value = 0;

	ret = peripheral_bus_gpio_read(pin, &read_value, user_data);
	peripheral_io_gdbus_gpio_complete_read(gpio, invocation, read_value, ret);

	return true;
}

gboolean handle_gpio_write(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gint value,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_write(pin, value, user_data);
	peripheral_io_gdbus_gpio_complete_write(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_get_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	gint edge;

	ret = peripheral_bus_gpio_get_edge(pin, &edge, user_data);
	peripheral_io_gdbus_gpio_complete_get_edge_mode(gpio, invocation, edge, ret);

	return true;
}

gboolean handle_gpio_set_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gint edge,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_set_edge(pin, edge, user_data);
	peripheral_io_gdbus_gpio_complete_set_edge_mode(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_register_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_register_irq(pin, user_data);
	peripheral_io_gdbus_gpio_complete_register_irq(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_unregister_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_bus_gpio_unregister_irq(pin, user_data);
	peripheral_io_gdbus_gpio_complete_unregister_irq(gpio, invocation, ret);

	return true;
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
	pb_i2c_data_h i2c_handle;

	ret = peripheral_bus_i2c_open(bus, address, &i2c_handle, user_data);

	if (ret == PERIPHERAL_ERROR_NONE) {
		if (peripheral_bus_get_client_info(invocation, pb_data, &i2c_handle->client_info) == 0)
			_D("bus : %d, address : %d, id = %s", bus, address, i2c_handle->client_info.id);
		else
			ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	peripheral_io_gdbus_i2c_complete_open(i2c, invocation, GPOINTER_TO_UINT(i2c_handle), ret);

	return true;
}

gboolean handle_i2c_close(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_i2c_data_h i2c_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!i2c_handle || !i2c_handle->client_info.id) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(i2c_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_i2c_close(i2c_handle, user_data);

out:
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
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_i2c_data_h i2c_handle = GUINT_TO_POINTER(handle);
	GVariant *data_array = NULL;
	const gchar *id;

	/* Handle validation */
	if (!i2c_handle || !i2c_handle->client_info.id) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(i2c_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_i2c_read(i2c_handle, length, &data_array);

out:
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
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_i2c_data_h i2c_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!i2c_handle || !i2c_handle->client_info.id) {
		_E("i2c handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(i2c_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_i2c_write(i2c_handle, length, data_array);

out:
	peripheral_io_gdbus_i2c_complete_write(i2c, invocation, ret);

	return true;
}

gboolean handle_pwm_open(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint device,
		gint channel,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle;

	ret = peripheral_bus_pwm_open(device, channel, &pwm_handle, user_data);

	if (ret == PERIPHERAL_ERROR_NONE) {
		if (peripheral_bus_get_client_info(invocation, pb_data, &pwm_handle->client_info) == 0)
			_D("device : %d, channel : %d, id = %s", device, channel, pwm_handle->client_info.id);
		else
			ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, GPOINTER_TO_UINT(pwm_handle), ret);

	return true;
}

gboolean handle_pwm_close(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_close(pwm_handle, user_data);
	}

	peripheral_io_gdbus_pwm_complete_close(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_set_period(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint period,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_set_period(pwm_handle, period);
	}

	peripheral_io_gdbus_pwm_complete_set_period(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_get_period(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	int period = 0;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_get_period(pwm_handle, &period);
	}

	peripheral_io_gdbus_pwm_complete_get_period(pwm, invocation, period, ret);

	return true;
}

gboolean handle_pwm_set_duty_cycle(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint duty_cycle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_set_duty_cycle(pwm_handle, duty_cycle);
	}

	peripheral_io_gdbus_pwm_complete_set_duty_cycle(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_get_duty_cycle(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	int duty_cycle = 0;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_get_duty_cycle(pwm_handle, &duty_cycle);
	}

	peripheral_io_gdbus_pwm_complete_get_duty_cycle(pwm, invocation, duty_cycle, ret);

	return true;
}

gboolean handle_pwm_set_polarity(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint polarity,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_set_polarity(pwm_handle, polarity);
	}

	peripheral_io_gdbus_pwm_complete_set_polarity(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_get_polarity(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	int polarity = 0;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_get_polarity(pwm_handle, &polarity);
	}

	peripheral_io_gdbus_pwm_complete_get_polarity(pwm, invocation, polarity, ret);

	return true;
}

gboolean handle_pwm_set_enable(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint enable,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_set_enable(pwm_handle, enable);
	}

	peripheral_io_gdbus_pwm_complete_set_enable(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_get_enable(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_pwm_data_h pwm_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	bool enable = false;

	/* Handle validation */
	if (!pwm_handle || !pwm_handle->client_info.id) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(pwm_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", pwm_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_pwm_get_enable(pwm_handle, &enable);
	}

	peripheral_io_gdbus_pwm_complete_get_enable(pwm, invocation, enable, ret);

	return true;
}

gboolean handle_uart_open(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint port,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle;

	ret = peripheral_bus_uart_open(port, &uart_handle, user_data);
	if (ret == PERIPHERAL_ERROR_NONE) {
		guint pid = 0;
		GError *error = NULL;
		GVariant *_ret;
		const gchar *id;

		id = g_dbus_method_invocation_get_sender(invocation);
		_ret = g_dbus_connection_call_sync(pb_data->connection,
			"org.freedesktop.DBus",
			"/org/freedesktop/DBus",
			"org.freedesktop.DBus",
			"GetConnectionUnixProcessID",
			g_variant_new("(s)", id),
			NULL,
			G_DBUS_CALL_FLAGS_NONE,
			-1,
			NULL,
			&error);

		if (_ret != NULL) {
			g_variant_get(_ret, "(u)", &pid);
			g_variant_unref(_ret);
		} else
			g_error_free(error);

		uart_handle->client_info.pid = (pid_t)pid;
		uart_handle->client_info.pgid = getpgid(pid);
		uart_handle->client_info.id = strdup(id);

		_D("port : %d, id = %s", port, uart_handle->client_info.id);
	}

	peripheral_io_gdbus_uart_complete_open(uart, invocation, GPOINTER_TO_UINT(uart_handle), ret);

	return true;
}

gboolean handle_uart_close(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_close(uart_handle, user_data);
	}

	peripheral_io_gdbus_uart_complete_close(uart, invocation, ret);

	return true;
}

gboolean handle_uart_flush(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_flush(uart_handle);
	}

	peripheral_io_gdbus_uart_complete_flush(uart, invocation, ret);

	return true;
}

gboolean handle_uart_set_baudrate(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint baudrate,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_set_baudrate(uart_handle, baudrate);
	}

	peripheral_io_gdbus_uart_complete_set_baudrate(uart, invocation, ret);

	return true;
}
gboolean handle_uart_set_mode(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint byte_size,
		guint parity,
		guint stop_bits,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_set_mode(uart_handle, byte_size, parity, stop_bits);
	}

	peripheral_io_gdbus_uart_complete_set_mode(uart, invocation, ret);

	return true;
}
gboolean handle_uart_set_flowcontrol(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gboolean xonxoff,
		gboolean rtscts,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_set_flowcontrol(uart_handle, xonxoff, rtscts);
	}

	peripheral_io_gdbus_uart_complete_set_flowcontrol(uart, invocation, ret);

	return true;
}

gboolean handle_uart_read(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	uint8_t err_buf[2] = {0, };
	GVariant *data_array = NULL;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_read(uart_handle, &data_array, length);
	}

	if (!data_array)
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	peripheral_io_gdbus_uart_complete_read(uart, invocation, data_array, ret);

	return true;
}

gboolean handle_uart_write(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_uart_data_h uart_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!uart_handle || !uart_handle->client_info.id) {
		_E("uart handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	} else {
		id = g_dbus_method_invocation_get_sender(invocation);
		if (strcmp(uart_handle->client_info.id, id)) {
			_E("Invalid access, handle id : %s, current id : %s", uart_handle->client_info.id, id);
			ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		} else
			ret = peripheral_bus_uart_write(uart_handle, data_array, length);
	}

	peripheral_io_gdbus_uart_complete_write(uart, invocation, ret);

	return true;
}

void peripheral_bus_emit_gpio_changed(PeripheralIoGdbusGpio *gpio,
									gint pin,
									gint state)
{
	g_assert(gpio != NULL);

	peripheral_io_gdbus_gpio_emit_gpio_changed(gpio, pin, state);
}

static gboolean __gpio_init(peripheral_bus_s *pb_data)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	pb_data->gpio_skeleton = peripheral_io_gdbus_gpio_skeleton_new();
	/* Register for method callbacks as signal callbacks */
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-open",
			G_CALLBACK(handle_gpio_open),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-close",
			G_CALLBACK(handle_gpio_close),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-get-direction",
			G_CALLBACK(handle_gpio_get_direction),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-set-direction",
			G_CALLBACK(handle_gpio_set_direction),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-read",
			G_CALLBACK(handle_gpio_read),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-write",
			G_CALLBACK(handle_gpio_write),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-get-edge-mode",
			G_CALLBACK(handle_gpio_get_edge_mode),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-set-edge-mode",
			G_CALLBACK(handle_gpio_set_edge_mode),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-register-irq",
			G_CALLBACK(handle_gpio_register_irq),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-unregister-irq",
			G_CALLBACK(handle_gpio_unregister_irq),
			pb_data);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_GPIO_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, pb_data->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(pb_data->gpio_skeleton),
		pb_data->connection, PERIPHERAL_GDBUS_GPIO_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __i2c_init(peripheral_bus_s *pb_data)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	pb_data->i2c_skeleton = peripheral_io_gdbus_i2c_skeleton_new();
	g_signal_connect(pb_data->i2c_skeleton,
			"handle-open",
			G_CALLBACK(handle_i2c_open),
			pb_data);
	g_signal_connect(pb_data->i2c_skeleton,
			"handle-close",
			G_CALLBACK(handle_i2c_close),
			pb_data);
	g_signal_connect(pb_data->i2c_skeleton,
			"handle-read",
			G_CALLBACK(handle_i2c_read),
			pb_data);
	g_signal_connect(pb_data->i2c_skeleton,
			"handle-write",
			G_CALLBACK(handle_i2c_write),
			pb_data);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_I2C_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, pb_data->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(pb_data->i2c_skeleton),
		pb_data->connection, PERIPHERAL_GDBUS_I2C_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __pwm_init(peripheral_bus_s *pb_data)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	pb_data->pwm_skeleton = peripheral_io_gdbus_pwm_skeleton_new();
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-open",
			G_CALLBACK(handle_pwm_open),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-close",
			G_CALLBACK(handle_pwm_close),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-period",
			G_CALLBACK(handle_pwm_set_period),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-get-period",
			G_CALLBACK(handle_pwm_get_period),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-duty-cycle",
			G_CALLBACK(handle_pwm_set_duty_cycle),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-get-duty-cycle",
			G_CALLBACK(handle_pwm_get_duty_cycle),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-polarity",
			G_CALLBACK(handle_pwm_set_polarity),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-get-polarity",
			G_CALLBACK(handle_pwm_get_polarity),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-enable",
			G_CALLBACK(handle_pwm_set_enable),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-get-enable",
			G_CALLBACK(handle_pwm_get_enable),
			pb_data);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_PWM_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, pb_data->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(pb_data->pwm_skeleton),
		pb_data->connection, PERIPHERAL_GDBUS_PWM_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __uart_init(peripheral_bus_s *pb_data)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	pb_data->uart_skeleton = peripheral_io_gdbus_uart_skeleton_new();
	g_signal_connect(pb_data->uart_skeleton,
			"handle-open",
			G_CALLBACK(handle_uart_open),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-close",
			G_CALLBACK(handle_uart_close),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-flush",
			G_CALLBACK(handle_uart_flush),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-baudrate",
			G_CALLBACK(handle_uart_set_baudrate),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-mode",
			G_CALLBACK(handle_uart_set_mode),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-flowcontrol",
			G_CALLBACK(handle_uart_set_flowcontrol),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-read",
			G_CALLBACK(handle_uart_read),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-write",
			G_CALLBACK(handle_uart_write),
			pb_data);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_UART_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, pb_data->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(pb_data->uart_skeleton),
		pb_data->connection, PERIPHERAL_GDBUS_UART_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static void on_bus_acquired(GDBusConnection *connection,
							const gchar *name,
							gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;

	pb_data->connection = connection;
	if (__gpio_init(pb_data) == FALSE)
		_E("Can not signal connect");

	if (__i2c_init(pb_data) == FALSE)
		_E("Can not signal connect");

	if (__pwm_init(pb_data) == FALSE)
		_E("Can not signal connect");

	if (__uart_init(pb_data) == FALSE)
		_E("Can not signal connect");
}

static void on_name_acquired(GDBusConnection *conn,
				const gchar *name, gpointer user_data)
{
}

static void on_name_lost(GDBusConnection *conn,
				const gchar *name, gpointer user_data)
{
	_E("Dbus name is lost!");
}

int main(int argc, char *argv[])
{
	GMainLoop *loop;
	guint owner_id = 0;
	peripheral_bus_s *pb_data;

	pb_data = (peripheral_bus_s*)calloc(1, sizeof(peripheral_bus_s));
	if (pb_data == NULL) {
		_E("failed to allocate peripheral_bus_s");
		return -1;
	}

	owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
							  PERIPHERAL_GDBUS_NAME,
							  (GBusNameOwnerFlags) (G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT
							  | G_BUS_NAME_OWNER_FLAGS_REPLACE),
							  on_bus_acquired,
							  on_name_acquired,
							  on_name_lost,
							  pb_data,
							  NULL);
	if (!owner_id) {
		_E("g_bus_own_name_error");
		free(pb_data);
		return -1;
	}

	loop = g_main_loop_new(NULL, FALSE);

	_D("Enter main loop!");
	g_main_loop_run(loop);

	if (pb_data)
		free(pb_data);

	if (loop != NULL)
		g_main_loop_unref(loop);

	return 0;
}
