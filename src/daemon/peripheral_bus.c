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
#include <peripheral_dbus.h>
#include <peripheral_internal.h>

#include "peripheral_bus.h"
#include "peripheral_bus_gpio.h"
#include "peripheral_bus_i2c.h"
#include "peripheral_bus_pwm.h"
#include "peripheral_common.h"


static GDBusNodeInfo *introspection_data = NULL;

static void handle_request_gpio(GDBusMethodInvocation *invocation,
								GVariant *parameters,
								gpointer user_data)
{
	gchar *function;
	gint32 ret = PERIPHERAL_ERROR_NONE;
	gint32 pin;
	gint32 direction;
	gint32 edge;
	gint32 read_value = 0;
	gint32 write_value;
	struct _peripheral_gpio_s st_gpio;

	g_variant_get(parameters, "(siiii)", &function, &pin, &direction, &edge, &write_value);

	st_gpio.pin = pin;
	st_gpio.edge = edge;
	st_gpio.direction = direction;

	ret = peripheral_bus_gpio_process(&st_gpio, function, write_value, &read_value);

	g_dbus_method_invocation_return_value(invocation,
									g_variant_new("(iiiii)", st_gpio.pin, st_gpio.direction, st_gpio.edge, read_value, ret));

	g_free(function);
}

static void handle_request_i2c(GDBusMethodInvocation *invocation,
							   GVariant *parameters,
							   gpointer user_data)
{
	gchar *function;
	gint32 ret = PERIPHERAL_ERROR_NONE;
	gint32 fd;
	GVariantIter *data;
	guint8 str;
	unsigned char data_value[100];
	GVariantBuilder *builder;
	gint32 value = 0;
	gint32 addr;
	int i = 0;
	struct _peripheral_i2c_s st_i2c;

	g_variant_get(parameters, "(siiayi)", &function, &value, &fd, &data, &addr);

	if (!strcmp(function, "READ") || !strcmp(function, "WRITE")) {
		while (g_variant_iter_loop(data, "y", &str)) {
			data_value[i] = str;
			i++;
		}
	}

	g_variant_iter_free(data);

	st_i2c.fd = fd;

	ret = peripheral_bus_i2c_process(&st_i2c, function, value, data_value, addr);

	builder = g_variant_builder_new(G_VARIANT_TYPE("ay"));

	if (!strcmp(function, "READ") || !strcmp(function, "WRITE")) {
		for (i = 0; i < value; i++)
			g_variant_builder_add(builder, "y", data_value[i]);
	} else {
		g_variant_builder_add(builder, "y", 0x10);
		g_variant_builder_add(builder, "y", 0x10);
	}
	g_dbus_method_invocation_return_value(invocation,
									g_variant_new("(iayi)", st_i2c.fd, builder, ret));

	g_free(function);
}

static void handle_request_pwm(GDBusMethodInvocation *invocation,
							   GVariant *parameters,
							   gpointer user_data)
{
	gchar *function;
	gint32 ret = PERIPHERAL_ERROR_NONE;
	struct _peripheral_pwm_s st_pwm;

	g_variant_get(parameters, "(siiiii)", &function, &st_pwm.device,
				&st_pwm.channel, &st_pwm.period, &st_pwm.duty_cycle, &st_pwm.enabled);

	ret = peripheral_bus_pwm_process(&st_pwm, function);

	g_dbus_method_invocation_return_value(invocation,
									 g_variant_new("(iii)", st_pwm.period, st_pwm.duty_cycle, ret));

	g_free(function);
}

static void handle_request_spi(GDBusMethodInvocation *invocation,
							   GVariant *parameters,
							   gpointer user_data)
{
}

static void handle_request_uart(GDBusMethodInvocation *invocation,
								GVariant *parameters,
								gpointer user_data)
{
}


static void handle_method_call(GDBusConnection *connection,
							   const gchar *sender,
							   const gchar *object_path,
							   const gchar *interface_name,
							   const gchar *method_name,
							   GVariant *parameters,
							   GDBusMethodInvocation *invocation,
							   gpointer user_data)
{

	if (parameters == NULL) {
		_E("Client : parameters Null !");
		return;
	}

	if (invocation == NULL) {
		_E("client : invocation NULL !");
		return;
	}

	if (method_name == NULL) {
		_E("Client : method_name NULL !");
		return;
	}

	if (!strcmp(method_name, PERIPHERAL_METHOD_GPIO))
		handle_request_gpio(invocation, parameters, user_data);
	else if (!strcmp(method_name, PERIPHERAL_METHOD_I2C))
		handle_request_i2c(invocation, parameters, user_data);
	else if (!strcmp(method_name, PERIPHERAL_METHOD_PWM))
		handle_request_pwm(invocation, parameters, user_data);
	else if (!strcmp(method_name, PERIPHERAL_METHOD_SPI))
		handle_request_spi(invocation, parameters, user_data);
	else if (!strcmp(method_name, PERIPHERAL_METHOD_UART))
		handle_request_uart(invocation, parameters, user_data);
}

static const GDBusInterfaceVTable interface_vtable = {
	handle_method_call,
	NULL,
	NULL,
};

guint registration_id = 0;

static void on_bus_acquired(GDBusConnection *connection,
							const gchar *name,
							gpointer user_data)
{
	guint registration_id;

	if (!connection) {
		_E("connection is null");
		return;
	}

	registration_id = g_dbus_connection_register_object(connection,
														PERIPHERAL_DBUS_PATH,
														introspection_data->interfaces[0],
														&interface_vtable,
														NULL,  /* user_data */
														NULL,  /* user_data_free_func */
														NULL); /* GError** */

	if (registration_id == 0)
		_E("Failed to g_dbus_connection_register_object");

	_D("Gdbus method call registered");
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
	guint owner_id;

	introspection_data = g_dbus_node_info_new_for_xml(peripheral_data_xml, NULL);
	g_assert(introspection_data != NULL);

	owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
							  PERIPHERAL_DBUS_NAME,
							  (GBusNameOwnerFlags) (G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT
							  | G_BUS_NAME_OWNER_FLAGS_REPLACE),
							  on_bus_acquired,
							  on_name_acquired,
							  on_name_lost,
							  NULL,
							  NULL);
	_D("owner_id : %d", owner_id);

	loop = g_main_loop_new(NULL, FALSE);

	_D("Enter main loop!");
	g_main_loop_run(loop);

	if (loop != NULL)
		g_main_loop_unref(loop);

	return 0;
}
