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
#include "peripheral_handle_pwm.h"
#include "peripheral_interface_pwm.h"
#include "peripheral_gdbus_pwm.h"

static void __pwm_on_name_vanished(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_h pwm_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(pwm_handle->watch_id);

	ret = peripheral_interface_pwm_unexport(pwm_handle->type.pwm.chip, pwm_handle->type.pwm.pin);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to unexport pwm");

	ret = peripheral_handle_pwm_destroy(pwm_handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to destroy pwm handle");
}

gboolean peripheral_gdbus_pwm_open(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint chip,
		gint pin,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h pwm_handle = NULL;
	GUnixFDList *pwm_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, info->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	ret = peripheral_interface_pwm_export(chip, pin);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to export pwm");
		goto out;
	}

	ret = peripheral_interface_pwm_fd_list_create(chip, pin, &pwm_fd_list);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create pwm fd list");
		peripheral_interface_pwm_unexport(chip, pin);
		goto out;
	}

	ret = peripheral_handle_pwm_create(chip, pin, &pwm_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create pwm handle");
		peripheral_interface_pwm_unexport(chip, pin);
		goto out;
	}

	pwm_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__pwm_on_name_vanished,
			pwm_handle,
			NULL);

out:
	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, pwm_fd_list, GPOINTER_TO_UINT(pwm_handle), ret);
	peripheral_interface_pwm_fd_list_destroy(pwm_fd_list);

	return true;
}

gboolean peripheral_gdbus_pwm_close(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_h pwm_handle = GUINT_TO_POINTER(handle);

	g_bus_unwatch_name(pwm_handle->watch_id);

	ret = peripheral_interface_pwm_unexport(pwm_handle->type.pwm.chip, pwm_handle->type.pwm.pin);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to unexport pwm");

	ret = peripheral_handle_pwm_destroy(pwm_handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to destroy pwm handle");

	peripheral_io_gdbus_pwm_complete_close(pwm, invocation, ret);

	return true;
}