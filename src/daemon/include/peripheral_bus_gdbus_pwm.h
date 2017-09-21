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

#ifndef __PERIPHERAL_BUS_GDBUS_PWM_H__
#define __PERIPHERAL_BUS_GDBUS_PWM_H__

#include "peripheral_io_gdbus.h"

gboolean handle_pwm_open(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint chip,
		gint pin,
		gpointer user_data);

gboolean handle_pwm_close(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_pwm_set_period(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint period,
		gpointer user_data);

gboolean handle_pwm_set_duty_cycle(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint duty_cycle,
		gpointer user_data);

gboolean handle_pwm_set_polarity(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint polarity,
		gpointer user_data);

gboolean handle_pwm_set_enable(
		PeripheralIoGdbusPwm *pwm,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint enable,
		gpointer user_data);

#endif /* __PERIPHERAL_BUS_GDBUS_PWM_H__ */
