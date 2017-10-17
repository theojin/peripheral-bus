/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gio/gio.h>

#include <cynara-creds-gdbus.h>
#include <cynara-client.h>
#include <cynara-session.h>

#include "peripheral_bus.h"
#include "peripheral_common.h"

#include "privilege_checker.h"

GVariant *peripheral_bus_build_variant_ay(uint8_t *data, int length)
{
	GVariantBuilder *builder;
	GVariant *variant;
	int i;

	if (data == NULL)
		return NULL;

	builder = g_variant_builder_new(G_VARIANT_TYPE("a(y)"));

	for (i = 0; i < length; i++)
		g_variant_builder_add(builder, "(y)", data[i]);

	variant = g_variant_new("a(y)", builder);
	g_variant_builder_unref(builder);

	return variant;
}

pb_data_h peripheral_bus_data_new(GList **plist)
{
	GList *list = *plist;
	pb_data_h handle;

	handle = (pb_data_h)calloc(1, sizeof(peripheral_bus_data_s));
	if (handle == NULL) {
		_E("failed to allocate peripheral_bus_data_s");
		return NULL;
	}

	*plist = g_list_append(list, handle);

	return handle;
}

int peripheral_bus_data_free(pb_data_h handle)
{
	GList *list = *handle->list;
	GList *link;

	RETVM_IF(handle == NULL, -1, "handle is null");

	link = g_list_find(list, handle);
	if (!link) {
		_E("handle does not exist in list");
		return -1;
	}

	*handle->list = g_list_remove_link(list, link);

	switch (handle->type) {
	case PERIPHERAL_BUS_TYPE_I2C:
		if (handle->dev.i2c.buffer)
			free(handle->dev.i2c.buffer);
		break;
	case PERIPHERAL_BUS_TYPE_UART:
		if (handle->dev.uart.buffer)
			free(handle->dev.uart.buffer);
		break;
	case PERIPHERAL_BUS_TYPE_SPI:
		if (handle->dev.spi.rx_buf)
			free(handle->dev.spi.rx_buf);
		if (handle->dev.spi.tx_buf)
			free(handle->dev.spi.tx_buf);
		break;
	default:
		break;
	}

	free(handle);
	g_list_free(link);

	return 0;
}

static int peripheral_bus_check_privilege(
		GDBusMethodInvocation *invocation,
		peripheral_bus_s *pb_data)
{
	int pid;
	const char *sender;
	char *session;
	char *client;
	char *user;

	sender = g_dbus_method_invocation_get_sender(invocation);

	cynara_creds_gdbus_get_pid(pb_data->connection, sender, &pid);
	session = cynara_session_from_pid(pid);

	cynara_creds_gdbus_get_client(pb_data->connection, sender, CLIENT_METHOD_DEFAULT, &client);
	cynara_creds_gdbus_get_user(pb_data->connection, sender, USER_METHOD_DEFAULT, &user);

	if (!session || !client || !user) {
		_E("Failed to get client info");
		return -1;
	}

	if (!peripheral_privilege_check(client, session, user, PERIPHERAL_PRIVILEGE)) {
		g_free(session);
		g_free(client);
		g_free(user);
		return -EACCES;
	}

	g_free(session);
	g_free(client);
	g_free(user);

	return 0;
}

int peripheral_bus_get_client_info(
		GDBusMethodInvocation *invocation,
		peripheral_bus_s *pb_data,
		pb_client_info_s *client_info)
{
	guint pid = 0;
	GError *error = NULL;
	GVariant *_ret;
	const gchar *id;
	int err;

	id = g_dbus_method_invocation_get_sender(invocation);
	if (id == NULL) {
		_E("Current id is NULL");
		return -1;
	}

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

	if ((err = peripheral_bus_check_privilege(invocation, pb_data)) < 0) {
		_E("Permission denied(%d)", pid);
		return err;
	}

	client_info->pid = (pid_t)pid;
	client_info->pgid = getpgid(pid);
	client_info->id = strdup(id);

	return 0;
}

int peripheral_bus_handle_is_valid(
		GDBusMethodInvocation *invocation,
		pb_data_h handle,
		GList *list)
{
	const gchar *id;

	if (!g_list_find(list, handle)) {
		_E("Cannot find handle");
		return -1;
	}

	id = g_dbus_method_invocation_get_sender(invocation);
	if (id == NULL) {
		_E("Current id is NULL");
		return -1;
	}

	if (strcmp(handle->client_info.id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", handle->client_info.id, id);
		return -1;
	}

	return 0;
}
