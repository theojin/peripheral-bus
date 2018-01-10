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
#include <systemd/sd-daemon.h>

#include <peripheral_io.h>

#include "peripheral_log.h"
#include "peripheral_privilege.h"
#include "peripheral_handle.h"
#include "peripheral_io_gdbus.h"
#include "peripheral_gdbus_gpio.h"
#include "peripheral_gdbus_i2c.h"
#include "peripheral_gdbus_pwm.h"
#include "peripheral_gdbus_spi.h"
#include "peripheral_gdbus_uart.h"
#include "peripheral_gdbus_adc.h"

#define PERIPHERAL_GDBUS_GPIO_PATH	"/Org/Tizen/Peripheral_io/Gpio"
#define PERIPHERAL_GDBUS_I2C_PATH	"/Org/Tizen/Peripheral_io/I2c"
#define PERIPHERAL_GDBUS_PWM_PATH	"/Org/Tizen/Peripheral_io/Pwm"
#define PERIPHERAL_GDBUS_UART_PATH	"/Org/Tizen/Peripheral_io/Uart"
#define PERIPHERAL_GDBUS_SPI_PATH	"/Org/Tizen/Peripheral_io/Spi"
#define PERIPHERAL_GDBUS_ADC_PATH   "/Org/Tizen/Peripheral_io/Adc"
#define PERIPHERAL_GDBUS_NAME		"org.tizen.peripheral_io"

static gboolean __gpio_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->gpio_skeleton = peripheral_io_gdbus_gpio_skeleton_new();
	/* Register for method callbacks as signal callbacks */
	g_signal_connect(info->gpio_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_gpio_open),
			info);
	g_signal_connect(info->gpio_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_gpio_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_GPIO_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->gpio_skeleton),
		info->connection, PERIPHERAL_GDBUS_GPIO_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __i2c_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->i2c_skeleton = peripheral_io_gdbus_i2c_skeleton_new();
	g_signal_connect(info->i2c_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_i2c_open),
			info);
	g_signal_connect(info->i2c_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_i2c_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_I2C_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->i2c_skeleton),
		info->connection, PERIPHERAL_GDBUS_I2C_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __pwm_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->pwm_skeleton = peripheral_io_gdbus_pwm_skeleton_new();
	g_signal_connect(info->pwm_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_pwm_open),
			info);
	g_signal_connect(info->pwm_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_pwm_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_PWM_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->pwm_skeleton),
		info->connection, PERIPHERAL_GDBUS_PWM_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __uart_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->uart_skeleton = peripheral_io_gdbus_uart_skeleton_new();
	g_signal_connect(info->uart_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_uart_open),
			info);
	g_signal_connect(info->uart_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_uart_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_UART_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->uart_skeleton),
		info->connection, PERIPHERAL_GDBUS_UART_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __spi_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->spi_skeleton = peripheral_io_gdbus_spi_skeleton_new();
	g_signal_connect(info->spi_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_spi_open),
			info);
	g_signal_connect(info->spi_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_spi_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_SPI_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->spi_skeleton),
		info->connection, PERIPHERAL_GDBUS_SPI_PATH, &error);

	if (ret == FALSE) {
		_E("Can not skeleton_export %s", error->message);
		g_error_free(error);
	}

	return true;
}

static gboolean __adc_init(peripheral_info_s *info)
{
	GDBusObjectManagerServer *manager;
	gboolean ret = FALSE;
	GError *error = NULL;

	/* Add interface to default object path */
	info->adc_skeleton = peripheral_io_gdbus_adc_skeleton_new();
	g_signal_connect(info->adc_skeleton,
			"handle-open",
			G_CALLBACK(peripheral_gdbus_adc_open),
			info);
	g_signal_connect(info->adc_skeleton,
			"handle-close",
			G_CALLBACK(peripheral_gdbus_adc_close),
			info);

	manager = g_dbus_object_manager_server_new(PERIPHERAL_GDBUS_ADC_PATH);

	/* Set connection to 'manager' */
	g_dbus_object_manager_server_set_connection(manager, info->connection);

	/* Export 'manager' interface on peripheral-io DBUS */
	ret = g_dbus_interface_skeleton_export(
		G_DBUS_INTERFACE_SKELETON(info->adc_skeleton),
		info->connection, PERIPHERAL_GDBUS_ADC_PATH, &error);

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
	peripheral_info_s *info = (peripheral_info_s*)user_data;

	info->connection = connection;

	if (__gpio_init(info) == FALSE)
		_E("Can not signal connect");

	if (__i2c_init(info) == FALSE)
		_E("Can not signal connect");

	if (__pwm_init(info) == FALSE)
		_E("Can not signal connect");

	if (__uart_init(info) == FALSE)
		_E("Can not signal connect");

	if (__spi_init(info) == FALSE)
		_E("Can not signal connect");

	if (__adc_init(info) == FALSE)
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

static gboolean peripheral_bus_notify(gpointer data)
{
	_D("sd_notify(READY=1)");
	sd_notify(0, "READY=1");

	return G_SOURCE_REMOVE;
}

int main(int argc, char *argv[])
{
	GMainLoop *loop;
	guint owner_id = 0;
	peripheral_info_s *info;

	info = (peripheral_info_s*)calloc(1, sizeof(peripheral_info_s));
	if (info == NULL) {
		_E("failed to allocate peripheral_info_s");
		return -1;
	}

	info->board = peripheral_bus_board_init();
	if (info->board == NULL) {
		_E("failed to init board");
		return -1;
	}

	owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
							  PERIPHERAL_GDBUS_NAME,
							  (GBusNameOwnerFlags) (G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT
							  | G_BUS_NAME_OWNER_FLAGS_REPLACE),
							  on_bus_acquired,
							  on_name_acquired,
							  on_name_lost,
							  info,
							  NULL);
	if (!owner_id) {
		_E("g_bus_own_name_error");
		free(info);
		return -1;
	}

	loop = g_main_loop_new(NULL, FALSE);

	g_idle_add(peripheral_bus_notify, NULL);

	peripheral_privilege_init();

	_D("Enter main loop!");
	g_main_loop_run(loop);

	peripheral_privilege_deinit();

	if (info) {
		peripheral_bus_board_deinit(info->board);
		free(info);
	}

	if (loop != NULL)
		g_main_loop_unref(loop);

	return 0;
}
