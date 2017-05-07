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

#include "i2c.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"

static pb_client_info_s* peripheral_bus_i2c_get_client_info(GDBusMethodInvocation *invocation, peripheral_bus_s *pb_data)
{
	pb_client_info_s *client_info;
	guint pid = 0;
	GError *error = NULL;
	GVariant *_ret;
	const gchar *id;

	client_info = (pb_client_info_s*)calloc(1, sizeof(pb_client_info_s));

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
	}
	g_error_free(error);

	client_info->pid = (pid_t)pid;
	client_info->pgid = getpgid(pid);
	client_info->id = strdup(id);

	return client_info;
}

static pb_i2c_data_h peripheral_bus_i2c_data_get(int bus, int address, GList **list)
{
	GList *i2c_list = *list;
	GList *link;
	pb_i2c_data_h i2c_data;

	if (i2c_list == NULL)
		return NULL;

	link = i2c_list;
	while (link) {
		i2c_data = (pb_i2c_data_h)link->data;
		if (i2c_data->bus == bus && i2c_data->address == address)
			return i2c_data;
		link = g_list_next(link);
	}

	return NULL;
}

static pb_i2c_data_h peripheral_bus_i2c_data_new(GList **list)
{
	GList *i2c_list = *list;
	pb_i2c_data_h i2c_data;

	i2c_data = (pb_i2c_data_h)calloc(1, sizeof(peripheral_bus_i2c_data_s));
	if (i2c_data == NULL) {
		_E("failed to allocate peripheral_bus_i2c_data_s");
		return NULL;
	}

	*list = g_list_append(i2c_list, i2c_data);

	return i2c_data;
}

static int peripheral_bus_i2c_data_free(pb_i2c_data_h i2c_handle, GList **list)
{
	GList *i2c_list = *list;
	GList *link;
	pb_i2c_data_h i2c_data;

	if (i2c_list == NULL)
		return -1;

	link = i2c_list;
	while (link) {
		i2c_data = (pb_i2c_data_h)link->data;

		if (i2c_data == i2c_handle) {
			*list = g_list_remove_link(i2c_list, link);
			if (i2c_data->client_info) {
				free(i2c_data->client_info->id);
				free(i2c_data->client_info);
			}
			free(i2c_data);
			g_list_free(link);
			return 0;
		}
		link = g_list_next(link);
	}

	return -1;
}

int peripheral_bus_i2c_open(GDBusMethodInvocation *invocation, int bus, int address, pb_i2c_data_h *i2c, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_i2c_data_h i2c_handle;
	int ret;
	int fd;

	if (peripheral_bus_i2c_data_get(bus, address, &pb_data->i2c_list)) {
		_E("Resource is in use, bus : %d, address : %d", bus, address);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = i2c_open(bus, &fd)) < 0)
		return ret;

	if ((ret = i2c_set_address(fd, address)) < 0) {
		i2c_close(fd);
		return ret;
	}

	i2c_handle = peripheral_bus_i2c_data_new(&pb_data->i2c_list);

	i2c_handle->fd = fd;
	i2c_handle->bus = bus;
	i2c_handle->address = address;
	i2c_handle->client_info = peripheral_bus_i2c_get_client_info(invocation, pb_data);
	*i2c = i2c_handle;

	_D("bus : %d, address : %d, pgid = %d, id = %s", bus, address,
		i2c_handle->client_info->pgid,
		i2c_handle->client_info->id);

	return ret;
}

int peripheral_bus_i2c_read(GDBusMethodInvocation *invocation, pb_i2c_data_h i2c, int length, unsigned char *data, gpointer user_data)
{
	const gchar *id;

	id = g_dbus_method_invocation_get_sender(invocation);

	if (strcmp(i2c->client_info->id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c->client_info->id, id);
		return PERIPHERAL_ERROR_INVALID_OPERATION;
	}

	return i2c_read(i2c->fd, data, length);
}

int peripheral_bus_i2c_write(GDBusMethodInvocation *invocation, pb_i2c_data_h i2c, int length, unsigned char *data, gpointer user_data)
{
	const gchar *id;

	id = g_dbus_method_invocation_get_sender(invocation);

	if (strcmp(i2c->client_info->id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c->client_info->id, id);
		return PERIPHERAL_ERROR_INVALID_OPERATION;
	}

	return i2c_write(i2c->fd, data, length);
}

int peripheral_bus_i2c_close(GDBusMethodInvocation *invocation, pb_i2c_data_h i2c, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	const gchar *id;
	int ret;

	_D("i2c_close, bus : %d, address : %d, pgid = %d", i2c->bus, i2c->address, i2c->client_info->pgid);

	id = g_dbus_method_invocation_get_sender(invocation);

	if (strcmp(i2c->client_info->id, id)) {
		_E("Invalid access, handle id : %s, current id : %s", i2c->client_info->id, id);
		return PERIPHERAL_ERROR_INVALID_OPERATION;
	}

	if ((ret = i2c_close(i2c->fd)) < 0)
		return ret;

	if (peripheral_bus_i2c_data_free(i2c, &pb_data->i2c_list) < 0)
		_E("Failed to free i2c data");

	return ret;
}
