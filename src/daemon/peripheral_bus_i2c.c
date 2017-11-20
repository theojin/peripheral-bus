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

#include "i2c.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

#define INITIAL_BUFFER_SIZE 128
#define MAX_BUFFER_SIZE 8192

static bool peripheral_bus_i2c_is_available(int bus, int address, peripheral_bus_s *pb_data)
{
	pb_board_dev_s *i2c = NULL;
	pb_data_h handle;
	GList *link;

	RETV_IF(pb_data == NULL, false);
	RETV_IF(pb_data->board == NULL, false);

	i2c = peripheral_bus_board_find_device(PB_BOARD_DEV_I2C, pb_data->board, bus);
	if (i2c == NULL) {
		_E("Not supported I2C bus : %d", bus);
		return false;
	}

	link = pb_data->i2c_list;
	while (link) {
		handle = (pb_data_h)link->data;
		if (handle->dev.i2c.bus == bus && handle->dev.i2c.address == address) {
			_E("Resource is in use, bus : %d, address : %d", bus, address);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_bus_i2c_open(int bus, int address, pb_data_h *handle, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h i2c_handle;
	int ret;
	int fd;

	if (!peripheral_bus_i2c_is_available(bus, address, pb_data)) {
		_E("bus : %d, address : 0x%x is not available", bus, address);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = i2c_open(bus, &fd)) < 0) {
		_E("i2c_open error (%d)", ret);
		goto open_err;
	}

	if ((ret = i2c_set_address(fd, address)) < 0) {
		_E("i2c_set_address error (%d)", ret);
		goto err;
	}

	i2c_handle = peripheral_bus_data_new(&pb_data->i2c_list);
	if (!i2c_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	i2c_handle->type = PERIPHERAL_BUS_TYPE_I2C;
	i2c_handle->list = &pb_data->i2c_list;
	i2c_handle->dev.i2c.fd = fd;
	i2c_handle->dev.i2c.bus = bus;
	i2c_handle->dev.i2c.address = address;
	i2c_handle->dev.i2c.buffer = malloc(INITIAL_BUFFER_SIZE);

	if (!(i2c_handle->dev.i2c.buffer)) {
		peripheral_bus_data_free(i2c_handle);
		_E("Failed to allocate data buffer");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}
	i2c_handle->dev.i2c.buffer_size = INITIAL_BUFFER_SIZE;

	*handle = i2c_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	i2c_close(fd);

open_err:
	return ret;
}

int peripheral_bus_i2c_close(pb_data_h handle)
{
	peripheral_bus_i2c_s *i2c = &handle->dev.i2c;
	int ret = PERIPHERAL_ERROR_NONE;

	_D("bus : %d, address : 0x%x, pgid = %d", i2c->bus, i2c->address, handle->client_info.pgid);

	if ((ret = i2c_close(i2c->fd)) < 0)
		return ret;

	if (peripheral_bus_data_free(handle) < 0) {
		_E("Failed to free i2c data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}
