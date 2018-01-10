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
#include "peripheral_handle_adc.h"
#include "peripheral_interface_adc.h"
#include "peripheral_gdbus_adc.h"

static void __adc_on_name_vanished(GDBusConnection *connection,
	const gchar *name,
	gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_h adc_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(adc_handle->watch_id);

	ret = peripheral_handle_adc_destroy(adc_handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to destroy adc handle");
}


gboolean peripheral_gdbus_adc_open(
		PeripheralIoGdbusAdc *adc,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint device,
		gint channel,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h adc_handle = NULL;
	GUnixFDList *adc_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, info->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	ret = peripheral_interface_adc_fd_list_create(device, channel, &adc_fd_list);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create adc fd list");
		goto out;
	}

	ret = peripheral_handle_adc_create(device, channel, &adc_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create adc handle");
		goto out;
	}

	adc_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__adc_on_name_vanished,
			adc_handle,
			NULL);

out:
	peripheral_io_gdbus_adc_complete_open(adc, invocation, adc_fd_list, GPOINTER_TO_UINT(adc_handle), ret);
	peripheral_interface_adc_fd_list_destroy(adc_fd_list);

	return true;
}

gboolean peripheral_gdbus_adc_close(
		PeripheralIoGdbusAdc *adc,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_h adc_handle = GUINT_TO_POINTER(handle);

	g_bus_unwatch_name(adc_handle->watch_id);

	ret = peripheral_handle_adc_destroy(adc_handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to destroy adc handle");

	peripheral_io_gdbus_adc_complete_close(adc, invocation, ret);

	return true;
}