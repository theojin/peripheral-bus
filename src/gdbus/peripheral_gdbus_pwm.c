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
#include "peripheral_gdbus_pwm.h"

static void __pwm_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	peripheral_h pwm_handle = (peripheral_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(pwm_handle->watch_id);
	peripheral_handle_pwm_destroy(pwm_handle);
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

	pwm_fd_list = g_unix_fd_list_new();
	if (pwm_fd_list == NULL) {
		_E("Failed to create pwm fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	ret = peripheral_handle_pwm_create(chip, pin, &pwm_handle, user_data);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to create peripheral pwm handle");
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(pwm_fd_list, pwm_handle->type.pwm.fd_period, NULL);
	g_unix_fd_list_append(pwm_fd_list, pwm_handle->type.pwm.fd_duty_cycle, NULL);
	g_unix_fd_list_append(pwm_fd_list, pwm_handle->type.pwm.fd_polarity, NULL);
	g_unix_fd_list_append(pwm_fd_list, pwm_handle->type.pwm.fd_enable, NULL);

	pwm_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__pwm_on_name_vanished,
			pwm_handle,
			NULL);

out:
	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, pwm_fd_list, GPOINTER_TO_UINT(pwm_handle), ret);

	if (pwm_fd_list != NULL)
		g_object_unref(pwm_fd_list);

	return true;
}
