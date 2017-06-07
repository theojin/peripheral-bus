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
#include "peripheral_bus_spi.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

static void __gpio_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_gpio_data_h gpio_handle = (pb_gpio_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(gpio_handle->watch_id);
	peripheral_bus_gpio_close(gpio_handle);
}

static void __i2c_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_i2c_data_h i2c_handle = (pb_i2c_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(i2c_handle->watch_id);
	peripheral_bus_i2c_close(i2c_handle);
}

static void __pwm_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_pwm_data_h pwm_handle = (pb_pwm_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(pwm_handle->watch_id);
	peripheral_bus_pwm_close(pwm_handle);
}


static void __uart_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_uart_data_h uart_handle = (pb_uart_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(uart_handle->watch_id);
	peripheral_bus_uart_close(uart_handle);
}

static void __spi_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_spi_data_h spi_handle = (pb_spi_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(spi_handle->watch_id);
	peripheral_bus_spi_close(spi_handle);
}

gboolean handle_gpio_open(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_gpio_data_h gpio_handle;

	if ((ret = peripheral_bus_gpio_open(pin, &gpio_handle, user_data)) < PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &gpio_handle->client_info) < 0) {
		peripheral_bus_gpio_close(gpio_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}

	gpio_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			gpio_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__gpio_on_name_vanished,
			gpio_handle,
			NULL);
	_D("gpio : %d, id = %s", gpio_handle->pin, gpio_handle->client_info.id);

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
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!gpio_handle || !gpio_handle->client_info.id) {
		_E("gpio handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}

	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(gpio_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", gpio_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	g_bus_unwatch_name(gpio_handle->watch_id);
	ret = peripheral_bus_gpio_close(gpio_handle);
out:
	peripheral_io_gdbus_gpio_complete_close(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_get_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);
	gint direction;

	ret = peripheral_bus_gpio_get_direction(gpio_handle, &direction);
	peripheral_io_gdbus_gpio_complete_get_direction(gpio, invocation, direction, ret);

	return true;
}

gboolean handle_gpio_set_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint direction,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);

	ret = peripheral_bus_gpio_set_direction(gpio_handle, direction);
	peripheral_io_gdbus_gpio_complete_set_direction(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_read(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);
	gint read_value = 0;

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
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);

	ret = peripheral_bus_gpio_write(gpio_handle, value);
	peripheral_io_gdbus_gpio_complete_write(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_get_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);
	gint edge;

	ret = peripheral_bus_gpio_get_edge(gpio_handle, &edge);
	peripheral_io_gdbus_gpio_complete_get_edge_mode(gpio, invocation, edge, ret);

	return true;
}

gboolean handle_gpio_set_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint edge,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);

	ret = peripheral_bus_gpio_set_edge(gpio_handle, edge);
	peripheral_io_gdbus_gpio_complete_set_edge_mode(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_register_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);

	ret = peripheral_bus_gpio_register_irq(gpio_handle);
	peripheral_io_gdbus_gpio_complete_register_irq(gpio, invocation, ret);

	return true;
}

gboolean handle_gpio_unregister_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_gpio_data_h gpio_handle = GUINT_TO_POINTER(handle);

	ret = peripheral_bus_gpio_unregister_irq(gpio_handle);
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

	g_bus_unwatch_name(i2c_handle->watch_id);
	ret = peripheral_bus_i2c_close(i2c_handle);

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
	uint8_t err_buf[2] = {0, };
	const gchar *id;

	/* Handle validation */
	if (!i2c_handle || !i2c_handle->client_info.id) {
		_E("i2c handle is not valid");
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(i2c_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c_handle->client_info.id, id);
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
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
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_i2c_data_h i2c_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	uint16_t data = 0xFFFF;

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

	ret = peripheral_bus_i2c_smbus_ioctl(i2c_handle, read_write, command, size, data_in, &data);
out:
	peripheral_io_gdbus_i2c_complete_smbus_ioctl(i2c, invocation, data, ret);

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

	if ((ret = peripheral_bus_pwm_open(device, channel, &pwm_handle, user_data)) <  PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &pwm_handle->client_info) < 0) {
		peripheral_bus_pwm_close(pwm_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}

	pwm_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			pwm_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__pwm_on_name_vanished,
			pwm_handle,
			NULL);
	_D("device : %d, channel : %d, id = %s", device, channel, pwm_handle->client_info.id);

out:
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
		} else {
			g_bus_unwatch_name(pwm_handle->watch_id);
			ret = peripheral_bus_pwm_close(pwm_handle);
		}
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

	if ((ret = peripheral_bus_uart_open(port, &uart_handle, user_data)) < PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &uart_handle->client_info) < 0) {
		peripheral_bus_uart_close(uart_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}

	uart_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			uart_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__uart_on_name_vanished,
			uart_handle,
			NULL);
	_D("port : %d, id = %s", port, uart_handle->client_info.id);

out:
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
		} else {
			g_bus_unwatch_name(uart_handle->watch_id);
			ret = peripheral_bus_uart_close(uart_handle);
		}
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

gboolean handle_spi_open(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint bus,
		gint cs,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle;

	ret = peripheral_bus_spi_open(bus, cs, &spi_handle, user_data);
	if (ret == PERIPHERAL_ERROR_NONE) {
		if (peripheral_bus_get_client_info(invocation, pb_data, &spi_handle->client_info) == 0)
			_D("bus : %d, cs : %d, id = %s", bus, cs, spi_handle->client_info.id);
		else
			ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	spi_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			spi_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__spi_on_name_vanished,
			spi_handle,
			NULL);

	peripheral_io_gdbus_spi_complete_open(spi, invocation, GPOINTER_TO_UINT(spi_handle), ret);

	return true;
}

gboolean handle_spi_close(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	g_bus_unwatch_name(spi_handle->watch_id);
	ret = peripheral_bus_spi_close(spi_handle);

out:
	peripheral_io_gdbus_spi_complete_close(spi, invocation, ret);

	return true;
}

gboolean handle_spi_set_mode(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar mode,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_set_mode(spi_handle, mode);

out:
	peripheral_io_gdbus_spi_complete_set_mode(spi, invocation, ret);

	return true;
}

gboolean handle_spi_get_mode(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	uint8_t mode = 0;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_get_mode(spi_handle, &mode);

out:
	peripheral_io_gdbus_spi_complete_get_mode(spi, invocation, mode, ret);

	return true;
}

gboolean handle_spi_set_lsb_first(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gboolean lsb,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_set_lsb_first(spi_handle, lsb);

out:
	peripheral_io_gdbus_spi_complete_set_lsb_first(spi, invocation, ret);

	return true;
}

gboolean handle_spi_get_lsb_first(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	gboolean lsb = 0;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_get_lsb_first(spi_handle, &lsb);

out:
	peripheral_io_gdbus_spi_complete_get_lsb_first(spi, invocation, lsb, ret);

	return true;
}

gboolean handle_spi_set_bits(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar bits,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_set_bits(spi_handle, bits);

out:
	peripheral_io_gdbus_spi_complete_set_bits(spi, invocation, ret);

	return true;
}

gboolean handle_spi_get_bits(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	uint8_t bits = 0;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_get_bits(spi_handle, &bits);

out:
	peripheral_io_gdbus_spi_complete_get_bits(spi, invocation, bits, ret);

	return true;
}

gboolean handle_spi_set_frequency(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint freq,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_set_frequency(spi_handle, freq);

out:
	peripheral_io_gdbus_spi_complete_set_frequency(spi, invocation, ret);

	return true;
}

gboolean handle_spi_get_frequency(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;
	unsigned int freq = 0;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_get_frequency(spi_handle, &freq);

out:
	peripheral_io_gdbus_spi_complete_get_frequency(spi, invocation, freq, ret);

	return true;
}

gboolean handle_spi_read(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	GVariant *data_array = NULL;
	uint8_t err_buf[2] = {0, };
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_read(spi_handle, &data_array, length);

out:
	peripheral_io_gdbus_spi_complete_read(spi, invocation, data_array, ret);

	return true;
}

gboolean handle_spi_write(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_write(spi_handle, data_array, length);

out:
	peripheral_io_gdbus_spi_complete_write(spi, invocation, ret);

	return true;
}

gboolean handle_spi_read_write(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *tx_data_array,
		gpointer user_data)
{
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_spi_data_h spi_handle = GUINT_TO_POINTER(handle);
	GVariant *rx_data_array = NULL;
	uint8_t err_buf[2] = {0, };
	const gchar *id;

	/* Handle validation */
	if (!spi_handle || !spi_handle->client_info.id) {
		_E("spi handle is not valid");
		rx_data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_UNKNOWN;
		goto out;
	}
	id = g_dbus_method_invocation_get_sender(invocation);
	if (strcmp(spi_handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", spi_handle->client_info.id, id);
		rx_data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));
		ret = PERIPHERAL_ERROR_INVALID_OPERATION;
		goto out;
	}

	ret = peripheral_bus_spi_read_write(spi_handle, tx_data_array, &rx_data_array, length);

out:
	peripheral_io_gdbus_spi_complete_read_write(spi, invocation, rx_data_array, ret);

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
	g_signal_connect(pb_data->i2c_skeleton,
			"handle-smbus-ioctl",
			G_CALLBACK(handle_i2c_smbus_ioctl),
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

static gboolean __spi_init(peripheral_bus_s *pb_data)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	pb_data->spi_skeleton = peripheral_io_gdbus_spi_skeleton_new();
	g_signal_connect(pb_data->spi_skeleton,
			"handle-open",
			G_CALLBACK(handle_spi_open),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-close",
			G_CALLBACK(handle_spi_close),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-mode",
			G_CALLBACK(handle_spi_set_mode),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-get-mode",
			G_CALLBACK(handle_spi_get_mode),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-lsb-first",
			G_CALLBACK(handle_spi_set_lsb_first),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-get-lsb-first",
			G_CALLBACK(handle_spi_get_lsb_first),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-bits",
			G_CALLBACK(handle_spi_set_bits),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-get-bits",
			G_CALLBACK(handle_spi_get_bits),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-frequency",
			G_CALLBACK(handle_spi_set_frequency),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-get-frequency",
			G_CALLBACK(handle_spi_get_frequency),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-write",
			G_CALLBACK(handle_spi_write),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-read",
			G_CALLBACK(handle_spi_read),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-read-write",
			G_CALLBACK(handle_spi_read_write),
			pb_data);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_SPI_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, pb_data->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(pb_data->spi_skeleton),
		pb_data->connection, PERIPHERAL_GDBUS_SPI_PATH, &error);

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

	if (__spi_init(pb_data) == FALSE)
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
