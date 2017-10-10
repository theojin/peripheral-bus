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

	_D("bus : %d, address : 0x%x", bus, address);

	if (!peripheral_bus_i2c_is_available(bus, address, pb_data))
		return PERIPHERAL_ERROR_RESOURCE_BUSY;

	if ((ret = i2c_open(bus, &fd)) < 0)
		return ret;

	if ((ret = i2c_set_address(fd, address)) < 0) {
		i2c_close(fd);
		return ret;
	}

	i2c_handle = peripheral_bus_data_new(&pb_data->i2c_list);

	i2c_handle->type = PERIPHERAL_BUS_TYPE_I2C;
	i2c_handle->list = &pb_data->i2c_list;
	i2c_handle->dev.i2c.fd = fd;
	i2c_handle->dev.i2c.bus = bus;
	i2c_handle->dev.i2c.address = address;
	i2c_handle->dev.i2c.buffer = malloc(INITIAL_BUFFER_SIZE);

	if (!(i2c_handle->dev.i2c.buffer)) {
		peripheral_bus_data_free(i2c_handle);
		i2c_close(fd);
		_E("Failed to allocate data buffer");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}
	i2c_handle->dev.i2c.buffer_size = INITIAL_BUFFER_SIZE;

	*handle = i2c_handle;

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

int peripheral_bus_i2c_read(pb_data_h handle, int length, GVariant **data_array)
{
	peripheral_bus_i2c_s *i2c = &handle->dev.i2c;
	uint8_t err_buf[2] = {0, };
	int ret;

	/* Limit maximum length */
	if (length > MAX_BUFFER_SIZE) length = MAX_BUFFER_SIZE;

	/* Increase buffer if needed */
	if (length > i2c->buffer_size) {
		uint8_t *new;
		new = (uint8_t*)realloc(i2c->buffer, length);

		if (!new) {
			ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
			_E("Failed to realloc buffer");
			goto out;
		}
		i2c->buffer = new;
		i2c->buffer_size = length;
	}

	ret = i2c_read(i2c->fd, i2c->buffer, length);
	if (ret < 0)
		_E("Read Failed, bus : %d, address : 0x%x", i2c->bus, i2c->address);

	*data_array = peripheral_bus_build_variant_ay(i2c->buffer, length);

	return ret;

out:
	*data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	return ret;
}

int peripheral_bus_i2c_write(pb_data_h handle, int length, GVariant *data_array)
{
	peripheral_bus_i2c_s *i2c = &handle->dev.i2c;
	GVariantIter *iter;
	guchar str;
	int ret, i = 0;

	/* Limit maximum length */
	if (length > MAX_BUFFER_SIZE) length = MAX_BUFFER_SIZE;

	/* Increase buffer if needed */
	if (length > i2c->buffer_size) {
		uint8_t *new;
		new = (uint8_t*)realloc(i2c->buffer, length);

		RETVM_IF(new == NULL, PERIPHERAL_ERROR_OUT_OF_MEMORY, "Failed to realloc buffer");
		i2c->buffer = new;
		i2c->buffer_size = length;
	}

	g_variant_get(data_array, "a(y)", &iter);
	while (g_variant_iter_loop(iter, "(y)", &str) && i < length)
		i2c->buffer[i++] = str;
	g_variant_iter_free(iter);

	ret = i2c_write(i2c->fd, i2c->buffer, length);
	if (ret < 0)
		_E("Write Failed, bus : %d, address : 0x%x", i2c->bus, i2c->address);

	return ret;
}

int peripheral_bus_i2c_smbus_ioctl(pb_data_h handle, uint8_t read_write, uint8_t command, uint32_t size, uint16_t data_in, uint16_t *data_out)
{
	peripheral_bus_i2c_s *i2c = &handle->dev.i2c;
	struct i2c_smbus_ioctl_data data_arg;
	union i2c_smbus_data data;
	int ret;

	memset(&data, 0x0, sizeof(data.block));

	data_arg.read_write = read_write;
	data_arg.size = size;
	data_arg.data = &data;

	RETV_IF(size < I2C_SMBUS_BYTE || size > I2C_SMBUS_WORD_DATA, PERIPHERAL_ERROR_INVALID_PARAMETER);
	RETV_IF(read_write > I2C_SMBUS_READ, PERIPHERAL_ERROR_INVALID_PARAMETER);

	if (read_write == I2C_SMBUS_WRITE) {
		data_arg.command = command;
		if (size == I2C_SMBUS_BYTE_DATA)
			data.byte = (uint8_t)data_in;
		else if (size == I2C_SMBUS_WORD_DATA)
			data.word = data_in;
		else if (size == I2C_SMBUS_BYTE)
			data_arg.command = (uint8_t)data_in; // Data should be set to command.
	} else if (read_write == I2C_SMBUS_READ && size != I2C_SMBUS_BYTE)  {
		data_arg.command = command;
	}

	ret = i2c_smbus_ioctl(i2c->fd, &data_arg);
	if (ret < 0)
		_E("Ioctl failed, bus : %d, address : 0x%x", i2c->bus, i2c->address);

	if (ret == 0 && read_write == I2C_SMBUS_READ) {
		if (size == I2C_SMBUS_BYTE_DATA || size == I2C_SMBUS_BYTE)
			*data_out = (uint8_t)data.byte;
		else if (size == I2C_SMBUS_WORD_DATA)
			*data_out = data.word;
	}

	return ret;
}
