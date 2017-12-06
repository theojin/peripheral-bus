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
#include "peripheral_handle_spi.h"
#include "peripheral_gdbus_spi.h"

static void __spi_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	peripheral_h spi_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(spi_handle->watch_id);
	peripheral_handle_spi_destroy(spi_handle);
}

gboolean peripheral_gdbus_spi_open(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint bus,
		gint cs,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h spi_handle = NULL;
	GUnixFDList *spi_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, info->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	spi_fd_list = g_unix_fd_list_new();
	if (spi_fd_list == NULL) {
		_E("Failed to create spi fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	ret = peripheral_handle_spi_create(bus, cs, &spi_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create peripheral spi handle");
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(spi_fd_list, spi_handle->type.spi.fd, NULL);

	spi_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__spi_on_name_vanished,
			spi_handle,
			NULL);

out:
	peripheral_io_gdbus_spi_complete_open(spi, invocation, spi_fd_list, GPOINTER_TO_UINT(spi_handle), ret);

	if (spi_fd_list != NULL)
		g_object_unref(spi_fd_list);

	return true;
}

gboolean peripheral_gdbus_spi_close(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_h spi_handle = GUINT_TO_POINTER(handle);

	g_bus_unwatch_name(spi_handle->watch_id);
	ret = peripheral_handle_spi_destroy(spi_handle);

	peripheral_io_gdbus_spi_complete_close(spi, invocation, ret);

	return true;
}