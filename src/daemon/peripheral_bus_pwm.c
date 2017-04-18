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

#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>

#include <peripheral_io.h>

#include "pwm.h"
#include "peripheral_common.h"

int peripheral_bus_pwm_open(int device, int channel)
{
	return pwm_open(device, channel);
}

int peripheral_bus_pwm_close(int device, int channel)
{
	return pwm_close(device, channel);
}

int peripheral_bus_pwm_setduty(int device, int channel, int duty_cycle)
{
	return pwm_set_duty_cycle(device, channel, duty_cycle);
}

int peripheral_bus_pwm_setperiod(int device, int channel, int period)
{
	return pwm_set_period(device, channel, period);
}

int peripheral_bus_pwm_setenable(int device, int channel, int enable)
{
	return pwm_set_enabled(device, channel, enable);
}

int peripheral_bus_pwm_getduty(int device, int channel, int * duty_cycle)
{
	return pwm_get_duty_cycle(device, channel, duty_cycle);
}

int peripheral_bus_pwm_getperiod(int device, int channel, int *period)
{
	return pwm_get_period(device, channel, period);
}

int peripheral_bus_pwm_process(peripheral_pwm_context_h dev, char *function)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (dev == NULL) {
		_E("dev null error");
		return PERIPHERAL_ERROR_INVALID_PARAMETER;
	}

	if (!g_strcmp0(function, "OPEN"))
		ret = peripheral_bus_pwm_open(dev->device, dev->channel);
	else if (!g_strcmp0(function, "CLOSE"))
		ret = peripheral_bus_pwm_close(dev->device, dev->channel);
	else if (!g_strcmp0(function, "SET_DUTY"))
		ret = peripheral_bus_pwm_setduty(dev->device, dev->channel, dev->duty_cycle);
	else if (!g_strcmp0(function, "SET_PERIOD"))
		ret = peripheral_bus_pwm_setperiod(dev->device, dev->channel, dev->period);
	else if (!g_strcmp0(function, "SET_ENABLE"))
		ret = peripheral_bus_pwm_setenable(dev->device, dev->channel, dev->enabled);
	else if (!g_strcmp0(function, "GET_DUTY"))
		ret = peripheral_bus_pwm_getduty(dev->device, dev->channel, &dev->duty_cycle);
	else if (!g_strcmp0(function, "GET_PERIOD"))
		ret = peripheral_bus_pwm_getperiod(dev->device, dev->channel, &dev->period);

	return ret;
}
