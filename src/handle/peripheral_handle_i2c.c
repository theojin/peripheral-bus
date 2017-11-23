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

#include "peripheral_interface_i2c.h"
#include "peripheral_handle_common.h"

#define INITIAL_BUFFER_SIZE 128
#define MAX_BUFFER_SIZE 8192

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

int peripheral_handle_i2c_create(int bus, int address, peripheral_h *handle, gpointer user_data)
{
	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h i2c_handle;
	int ret;

	if (!__peripheral_handle_i2c_is_creatable(bus, address, info)) {
		_E("bus : %d, address : 0x%x is not available", bus, address);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	// TODO : make fd list using the interface function

	i2c_handle = peripheral_handle_new(&info->i2c_list);
	if (!i2c_handle) {
		_E("peripheral_handle_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	i2c_handle->list = &info->i2c_list;
	i2c_handle->type.i2c.bus = bus;
	i2c_handle->type.i2c.address = address;

	*handle = i2c_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	return ret;
}

int peripheral_handle_i2c_destroy(peripheral_h handle)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (peripheral_handle_free(handle) < 0) {
		_E("Failed to free i2c data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}
