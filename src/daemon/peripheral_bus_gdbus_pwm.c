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
		gint chip,
		gint pin,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle;

	if ((ret = peripheral_bus_pwm_open(chip, pin, &pwm_handle, user_data)) <  PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &pwm_handle->client_info) < 0) {
		peripheral_bus_pwm_close(pwm_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
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
	peripheral_io_gdbus_pwm_complete_open(pwm, invocation, GPOINTER_TO_UINT(pwm_handle), ret);

	return true;
}

gboolean handle_pwm_close(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, pwm_handle, pb_data->pwm_list) != 0) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else {
		g_bus_unwatch_name(pwm_handle->watch_id);
		ret = peripheral_bus_pwm_close(pwm_handle);
	}

	peripheral_io_gdbus_pwm_complete_close(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_set_period(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint period,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, pwm_handle, pb_data->pwm_list) != 0) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_pwm_set_period(pwm_handle, period);

	peripheral_io_gdbus_pwm_complete_set_period(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_set_duty_cycle(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint duty_cycle,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, pwm_handle, pb_data->pwm_list) != 0) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_pwm_set_duty_cycle(pwm_handle, duty_cycle);

	peripheral_io_gdbus_pwm_complete_set_duty_cycle(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_set_polarity(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint polarity,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, pwm_handle, pb_data->pwm_list) != 0) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_pwm_set_polarity(pwm_handle, polarity);

	peripheral_io_gdbus_pwm_complete_set_polarity(pwm, invocation, ret);

	return true;
}

gboolean handle_pwm_set_enable(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint enable,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h pwm_handle = GUINT_TO_POINTER(handle);

	if (peripheral_bus_handle_is_valid(invocation, pwm_handle, pb_data->pwm_list) != 0) {
		_E("pwm handle is not valid");
		ret = PERIPHERAL_ERROR_INVALID_PARAMETER;
	} else
		ret = peripheral_bus_pwm_set_enable(pwm_handle, enable);

	peripheral_io_gdbus_pwm_complete_set_enable(pwm, invocation, ret);

	return true;
}
