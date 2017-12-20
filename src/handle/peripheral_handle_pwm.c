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

#include "peripheral_handle_common.h"

static bool __peripheral_handle_pwm_is_creatable(int chip, int pin, peripheral_info_s *info)
{
	pb_board_dev_s *pwm = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	pwm = peripheral_bus_board_find_device(PB_BOARD_DEV_PWM, info->board, chip, pin);
	if (pwm == NULL) {
		_E("Not supported PWM chip : %d, pin : %d", chip, pin);
		return false;
	}

	link = info->pwm_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.pwm.chip == chip && handle->type.pwm.pin == pin) {
			_E("Resource is in use, chip : %d, pin : %d", chip, pin);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_pwm_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to free pwm handle");

	return ret;
}

int peripheral_handle_pwm_create(int chip, int pin, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm handle");

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h pwm_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_pwm_is_creatable(chip, pin, info);
	if (is_handle_creatable == false) {
		_E("pwm %d.%d is not available", chip, pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	pwm_handle = peripheral_handle_new(&info->pwm_list);
	if (pwm_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	pwm_handle->list = &info->pwm_list;
	pwm_handle->type.pwm.chip = chip;
	pwm_handle->type.pwm.pin = pin;

	*handle = pwm_handle;

	return PERIPHERAL_ERROR_NONE;
}
