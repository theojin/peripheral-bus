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

static bool __peripheral_handle_gpio_is_creatable(int pin, peripheral_info_s *info)
{
	pb_board_dev_s *gpio = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	gpio = peripheral_bus_board_find_device(PB_BOARD_DEV_GPIO, info->board, pin);
	if (gpio == NULL) {
		_E("Not supported GPIO pin : %d", pin);
		return false;
	}

	link = info->gpio_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.gpio.pin == pin) {
			_E("gpio %d is busy", pin);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_gpio_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to free gpio handle");

	return ret;
}

int peripheral_handle_gpio_create(int pin, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio handle");

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h gpio_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_gpio_is_creatable(pin, info);
	if (is_handle_creatable == false) {
		_E("gpio %d is not available", pin);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	gpio_handle = peripheral_handle_new(&info->gpio_list);
	if (gpio_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	gpio_handle->list = &info->gpio_list;
	gpio_handle->type.gpio.pin = pin;

	*handle = gpio_handle;

	return PERIPHERAL_ERROR_NONE;
}
