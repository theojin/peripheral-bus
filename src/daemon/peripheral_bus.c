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
#include <peripheral_gdbus.h>

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_gdbus_gpio.h"
#include "peripheral_bus_gdbus_i2c.h"
#include "peripheral_bus_gdbus_pwm.h"
#include "peripheral_bus_gdbus_spi.h"
#include "peripheral_bus_gdbus_uart.h"

#include "privilege_checker.h"

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
			"handle-set-direction",
			G_CALLBACK(handle_gpio_set_direction),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-set-edge-mode",
			G_CALLBACK(handle_gpio_set_edge_mode),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-set-interrupted-cb",
			G_CALLBACK(handle_gpio_set_interrupted_cb),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-unset-interrupted-cb",
			G_CALLBACK(handle_gpio_unset_interrupted_cb),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-read",
			G_CALLBACK(handle_gpio_read),
			pb_data);
	g_signal_connect(pb_data->gpio_skeleton,
			"handle-write",
			G_CALLBACK(handle_gpio_write),
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
			"handle-set-duty-cycle",
			G_CALLBACK(handle_pwm_set_duty_cycle),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-polarity",
			G_CALLBACK(handle_pwm_set_polarity),
			pb_data);
	g_signal_connect(pb_data->pwm_skeleton,
			"handle-set-enable",
			G_CALLBACK(handle_pwm_set_enable),
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
			"handle-set-baud-rate",
			G_CALLBACK(handle_uart_set_baud_rate),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-byte-size",
			G_CALLBACK(handle_uart_set_byte_size),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-parity",
			G_CALLBACK(handle_uart_set_parity),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-stop-bits",
			G_CALLBACK(handle_uart_set_stop_bits),
			pb_data);
	g_signal_connect(pb_data->uart_skeleton,
			"handle-set-flow-control",
			G_CALLBACK(handle_uart_set_flow_control),
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
			"handle-set-bit-order",
			G_CALLBACK(handle_spi_set_bit_order),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-bits-per-word",
			G_CALLBACK(handle_spi_set_bits_per_word),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-set-frequency",
			G_CALLBACK(handle_spi_set_frequency),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-read",
			G_CALLBACK(handle_spi_read),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-write",
			G_CALLBACK(handle_spi_write),
			pb_data);
	g_signal_connect(pb_data->spi_skeleton,
			"handle-transfer",
			G_CALLBACK(handle_spi_transfer),
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
	peripheral_bus_s *pb_data;

	pb_data = (peripheral_bus_s*)calloc(1, sizeof(peripheral_bus_s));
	if (pb_data == NULL) {
		_E("failed to allocate peripheral_bus_s");
		return -1;
	}

	pb_data->board = peripheral_bus_board_init();
	if (pb_data->board == NULL) {
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
							  pb_data,
							  NULL);
	if (!owner_id) {
		_E("g_bus_own_name_error");
		free(pb_data);
		return -1;
	}

	loop = g_main_loop_new(NULL, FALSE);

	g_idle_add(peripheral_bus_notify, NULL);

	peripheral_privilege_init();

	_D("Enter main loop!");
	g_main_loop_run(loop);

	peripheral_privilege_deinit();

	if (pb_data) {
		peripheral_bus_board_deinit(pb_data->board);
		free(pb_data);
	}

	if (loop != NULL)
		g_main_loop_unref(loop);

	return 0;
}
