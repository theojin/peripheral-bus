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
#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"

static pb_pwm_data_h peripheral_bus_pwm_data_get(int device, int channel, GList **list)
{
	GList *pwm_list = *list;
	GList *link;
	pb_pwm_data_h pwm_data;

	link = pwm_list;
	while (link) {
		pwm_data = (pb_pwm_data_h)link->data;
		if (pwm_data->device == device && pwm_data->channel == channel)
			return pwm_data;
		link = g_list_next(link);
	}

	return NULL;
}

static pb_pwm_data_h peripheral_bus_pwm_data_new(GList **list)
{
	GList *pwm_list = *list;
	pb_pwm_data_h pwm_data;

	pwm_data = (pb_pwm_data_h)calloc(1, sizeof(peripheral_bus_pwm_data_s));
	if (pwm_data == NULL) {
		_E("failed to allocate peripheral_bus_pwm_data_s");
		return NULL;
	}

	*list = g_list_append(pwm_list, pwm_data);

	return pwm_data;
}

static int peripheral_bus_pwm_data_free(pb_pwm_data_h pwm_handle, GList **list)
{
	GList *pwm_list = *list;
	GList *link;
	pb_pwm_data_h pwm_data;

	link = pwm_list;
	while (link) {
		pwm_data = (pb_pwm_data_h)link->data;

		if (pwm_data == pwm_handle) {
			*list = g_list_remove_link(pwm_list, link);
			free(pwm_data);
			g_list_free(link);
			return 0;
		}
		link = g_list_next(link);
	}

	return -1;
}

int peripheral_bus_pwm_open(int device, int channel, pb_pwm_data_h *pwm, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_pwm_data_h pwm_handle;
	int ret;

	if (peripheral_bus_pwm_data_get(device, channel, &pb_data->pwm_list)) {
		_E("Resource is in use, device : %d, channel : %d", device, channel);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = pwm_open(device, channel)) < 0)
		goto open_err;

	pwm_handle = peripheral_bus_pwm_data_new(&pb_data->pwm_list);
	if (!pwm_handle) {
		_E("peripheral_bus_pwm_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	pwm_handle->list = &pb_data->pwm_list;
	pwm_handle->device = device;
	pwm_handle->channel = channel;
	*pwm = pwm_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	pwm_close(device, channel);

open_err:
	return ret;
}

int peripheral_bus_pwm_close(pb_pwm_data_h pwm)
{
	int ret;

	if ((ret = pwm_close(pwm->device, pwm->channel)) < 0) {
		_E("gpio_close error (%d)", ret);
		return ret;
	}

	peripheral_bus_pwm_data_free(pwm, pwm->list);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_pwm_set_period(pb_pwm_data_h pwm, int period)
{
	return pwm_set_period(pwm->device, pwm->channel, period);
}

int peripheral_bus_pwm_get_period(pb_pwm_data_h pwm, int *period)
{
	return pwm_get_period(pwm->device, pwm->channel, period);
}

int peripheral_bus_pwm_set_duty_cycle(pb_pwm_data_h pwm, int duty_cycle)
{
	return pwm_set_duty_cycle(pwm->device, pwm->channel, duty_cycle);
}

int peripheral_bus_pwm_get_duty_cycle(pb_pwm_data_h pwm, int *duty_cycle)
{
	return pwm_get_duty_cycle(pwm->device, pwm->channel, duty_cycle);
}

int peripheral_bus_pwm_set_polarity(pb_pwm_data_h pwm, int polarity)
{
	return pwm_set_polarity(pwm->device, pwm->channel, (pwm_polarity_e)polarity);
}

int peripheral_bus_pwm_get_polarity(pb_pwm_data_h pwm, int *polarity)
{
	return pwm_get_polarity(pwm->device, pwm->channel, (pwm_polarity_e*)polarity);
}

int peripheral_bus_pwm_set_enable(pb_pwm_data_h pwm, bool enable)
{
	return pwm_set_enable(pwm->device, pwm->channel, enable);
}

int peripheral_bus_pwm_get_enable(pb_pwm_data_h pwm, bool *enable)
{
	return pwm_get_enable(pwm->device, pwm->channel, enable);
}
