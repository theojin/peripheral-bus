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
	pb_data_h pwm_handle = (pb_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(pwm_handle->watch_id);
	peripheral_handle_pwm_destroy(pwm_handle);
}

gboolean handle_pwm_open(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint chip,
		gint pin,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = NULL;

	GUnixFDList *pwm_fd_list = NULL;

	ret = peripheral_privilege_check(invocation, pb_data->connection);
	if (ret != 0) {
		_E("Permission denied.");
		ret = PERIPHERAL_ERROR_PERMISSION_DENIED;
		goto out;
	}

	if ((ret = peripheral_handle_pwm_create(chip, pin, &pwm_handle, user_data)) <  PERIPHERAL_ERROR_NONE)
		goto out;

	pwm_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM,
			g_dbus_method_invocation_get_sender(invocation),
			G_BUS_NAME_WATCHER_FLAGS_NONE,
			NULL,
			__pwm_on_name_vanished,
			pwm_handle,
			NULL);

out:
	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, pwm_fd_list, GPOINTER_TO_UINT(pwm_handle), ret);

	return true;
}
