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
#include "peripheral_handle_uart.h"
#include "peripheral_gdbus_uart.h"

static void __uart_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	peripheral_h uart_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(uart_handle->watch_id);
	peripheral_handle_uart_destroy(uart_handle);
}

gboolean peripheral_gdbus_uart_open(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint port,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h uart_handle = NULL;
	GUnixFDList *uart_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, info->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	uart_fd_list = g_unix_fd_list_new();
	if (uart_fd_list == NULL) {
		_E("Failed to create uart fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	ret = peripheral_handle_uart_create(port, &uart_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create peripheral uart handle");
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(uart_fd_list, uart_handle->type.uart.fd, NULL);

	uart_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__uart_on_name_vanished,
			uart_handle,
			NULL);

out:
	peripheral_io_gdbus_uart_complete_open(uart, invocation, uart_fd_list, GPOINTER_TO_UINT(uart_handle), ret);

	if (uart_fd_list != NULL)
		g_object_unref(uart_fd_list);

	return true;
}