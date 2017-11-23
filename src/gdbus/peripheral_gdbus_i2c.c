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
#include <gio/gunixfdlist.h>

#include "peripheral_log.h"
#include "peripheral_privilege.h"
#include "peripheral_io_gdbus.h"
#include "peripheral_handle.h"
#include "peripheral_handle_i2c.h"
#include "peripheral_gdbus_i2c.h"

static void __i2c_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	peripheral_h i2c_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(i2c_handle->watch_id);
	peripheral_handle_i2c_destroy(i2c_handle);
}

gboolean peripheral_gdbus_i2c_open(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint bus,
		gint address,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h i2c_handle = NULL;
	GUnixFDList *i2c_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, info->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	i2c_fd_list = g_unix_fd_list_new();
	if (i2c_fd_list == NULL) {
		_E("Failed to create i2c fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	ret = peripheral_handle_i2c_create(bus, address, &i2c_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create periphreal i2c handle");
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(i2c_fd_list, i2c_handle->type.i2c.fd, NULL);

	i2c_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__i2c_on_name_vanished,
			i2c_handle,
			NULL);

out:
	peripheral_io_gdbus_i2c_complete_open(i2c, invocation, i2c_fd_list, GPOINTER_TO_UINT(i2c_handle), ret);

	if (i2c_fd_list != NULL)
		g_object_unref(i2c_fd_list);

	return true;
}
