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
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

static bool peripheral_bus_pwm_is_available(int chip, int pin, peripheral_bus_s *pb_data)
{
	pb_board_dev_s *pwm = NULL;
	pb_data_h handle;
	GList *link;

	RETV_IF(pb_data == NULL, false);
	RETV_IF(pb_data->board == NULL, false);

	pwm = peripheral_bus_board_find_device(PB_BOARD_DEV_PWM, pb_data->board, chip, pin);
	if (pwm == NULL) {
		_E("Not supported PWM chip : %d, pin : %d", chip, pin);
		return false;
	}

	link = pb_data->pwm_list;
	while (link) {
		handle = (pb_data_h)link->data;
		if (handle->dev.pwm.chip == chip && handle->dev.pwm.pin == pin) {
			_E("Resource is in use, chip : %d, pin : %d", chip, pin);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_bus_pwm_open(int chip, int pin, pb_data_h *handle, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h pwm_handle;
	int ret;

	if (!peripheral_bus_pwm_is_available(chip, pin, pb_data)) {
		_E("pwm %d.%d is not available", chip, pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = pwm_open(chip, pin)) < 0)
		goto open_err;

	pwm_handle = peripheral_bus_data_new(&pb_data->pwm_list);
	if (!pwm_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	pwm_handle->type = PERIPHERAL_BUS_TYPE_PWM;
	pwm_handle->list = &pb_data->pwm_list;
	pwm_handle->dev.pwm.chip = chip;
	pwm_handle->dev.pwm.pin = pin;
	*handle = pwm_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	pwm_close(chip, pin);

open_err:
	return ret;
}

int peripheral_bus_pwm_close(pb_data_h handle)
{
	peripheral_bus_pwm_s *pwm = &handle->dev.pwm;
	int ret = PERIPHERAL_ERROR_NONE;

	if ((ret = pwm_close(pwm->chip, pwm->pin)) < 0)
		return ret;

	if (peripheral_bus_data_free(handle) < 0) {
		_E("Failed to free i2c data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}

int peripheral_bus_pwm_set_period(pb_data_h handle, int period)
{
	peripheral_bus_pwm_s *pwm = &handle->dev.pwm;

	return pwm_set_period(pwm->chip, pwm->pin, period);
}


int peripheral_bus_pwm_set_duty_cycle(pb_data_h handle, int duty_cycle)
{
	peripheral_bus_pwm_s *pwm = &handle->dev.pwm;

	return pwm_set_duty_cycle(pwm->chip, pwm->pin, duty_cycle);
}


int peripheral_bus_pwm_set_polarity(pb_data_h handle, int polarity)
{
	peripheral_bus_pwm_s *pwm = &handle->dev.pwm;

	return pwm_set_polarity(pwm->chip, pwm->pin, (pwm_polarity_e)polarity);
}


int peripheral_bus_pwm_set_enable(pb_data_h handle, bool enable)
{
	peripheral_bus_pwm_s *pwm = &handle->dev.pwm;

	return pwm_set_enable(pwm->chip, pwm->pin, enable);
}