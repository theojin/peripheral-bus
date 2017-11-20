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

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_bus_pwm.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"
#include "peripheral_bus_gdbus_pwm.h"

static void __pwm_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_data_h pwm_handle = (pb_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(pwm_handle->watch_id);
	peripheral_bus_pwm_close(pwm_handle);
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

	if ((ret = peripheral_bus_pwm_open(chip, pin, &pwm_handle, user_data)) <  PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &pwm_handle->client_info) < 0) {
		peripheral_bus_pwm_close(pwm_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		pwm_handle = NULL;
		goto out;
	}

	pwm_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			pwm_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__pwm_on_name_vanished,
			pwm_handle,
			NULL);
	_D("chip : %d, pin : %d, id = %s", chip, pin, pwm_handle->client_info.id);

out:
	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, pwm_fd_list, GPOINTER_TO_UINT(pwm_handle), ret);

	return true;
}
