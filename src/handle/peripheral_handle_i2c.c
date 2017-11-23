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

#include "peripheral_interface_i2c.h"
#include "peripheral_handle_common.h"

static bool __peripheral_handle_i2c_is_creatable(int bus, int address, peripheral_info_s *info)
{
	pb_board_dev_s *i2c = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	i2c = peripheral_bus_board_find_device(PB_BOARD_DEV_I2C, info->board, bus);
	if (i2c == NULL) {
		_E("Not supported I2C bus : %d", bus);
		return false;
	}

	link = info->i2c_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.i2c.bus == bus && handle->type.i2c.address == address) {
			_E("Resource is in use, bus : %d, address : %d", bus, address);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_i2c_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_interface_i2c_fd_close(handle->type.i2c.fd);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to i2c close fd");

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to free i2c handle");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_handle_i2c_create(int bus, int address, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c bus");
	RETVM_IF(address < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c address");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c handle");

	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h i2c_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_i2c_is_creatable(bus, address, info);
	if (is_handle_creatable == false) {
		_E("bus : %d, address : 0x%x is not available", bus, address);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	i2c_handle = peripheral_handle_new(&info->i2c_list);
	if (i2c_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	i2c_handle->list = &info->i2c_list;
	i2c_handle->type.i2c.bus = bus;
	i2c_handle->type.i2c.address = address;
	i2c_handle->type.i2c.fd = -1;

	ret = peripheral_interface_i2c_fd_open(bus, address, &i2c_handle->type.i2c.fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to i2c fd open");
		goto out;
	}

	*handle = i2c_handle;
	return PERIPHERAL_ERROR_NONE;

out:
	peripheral_handle_i2c_destroy(i2c_handle);
	return ret;
}
