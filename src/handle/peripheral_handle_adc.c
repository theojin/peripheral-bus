/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

static bool __peripheral_handle_adc_is_creatable(int device, int channel, peripheral_info_s *info)
{
	pb_board_dev_s *adc = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	adc = peripheral_bus_board_find_device(PB_BOARD_DEV_ADC, info->board, device, channel);
	if (adc == NULL) {
		_E("Not supported ADC device : %d, channel : %d", device, channel);
		return false;
	}

	link = info->adc_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.adc.device == device && handle->type.adc.channel == channel) {
			_E("Resource is in use, device : %d, channel : %d", device, channel);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_adc_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to free adc handle");

	return ret;
}

int peripheral_handle_adc_create(int device, int channel, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(device < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc device");
	RETVM_IF(channel < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc channel");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc handle");

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h adc_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_adc_is_creatable(device, channel, info);
	if (is_handle_creatable == false) {
		_E("device : %d, channel : 0x%x is not available", device, channel);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	adc_handle = peripheral_handle_new(&info->adc_list);
	if (adc_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	adc_handle->list = &info->adc_list;
	adc_handle->type.adc.device = device;
	adc_handle->type.adc.channel = channel;

	*handle = adc_handle;

	return PERIPHERAL_ERROR_NONE;
}
