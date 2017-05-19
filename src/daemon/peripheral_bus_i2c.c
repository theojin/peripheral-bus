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

static pb_i2c_data_h peripheral_bus_i2c_data_get(int bus, int address, GList **list)
{
	GList *i2c_list = *list;
	GList *link;
	pb_i2c_data_h i2c_data;

	if (i2c_list == NULL)
		return NULL;

	link = i2c_list;
	while (link) {
		i2c_data = (pb_i2c_data_h)link->data;
		if (i2c_data->bus == bus && i2c_data->address == address)
			return i2c_data;
		link = g_list_next(link);
	}

	return NULL;
}

static pb_i2c_data_h peripheral_bus_i2c_data_new(GList **list)
{
	GList *i2c_list = *list;
	pb_i2c_data_h i2c_data;

	i2c_data = (pb_i2c_data_h)calloc(1, sizeof(peripheral_bus_i2c_data_s));
	if (i2c_data == NULL) {
		_E("failed to allocate peripheral_bus_i2c_data_s");
		return NULL;
	}

	*list = g_list_append(i2c_list, i2c_data);

	return i2c_data;
}

static int peripheral_bus_i2c_data_free(pb_i2c_data_h i2c_handle, GList **list)
{
	GList *i2c_list = *list;
	GList *link;
	pb_i2c_data_h i2c;

	if (i2c_handle == NULL)
		return -1;

	link = i2c_list;
	while (link) {
		i2c = (pb_i2c_data_h)link->data;

		if (i2c == i2c_handle) {
			*list = g_list_remove_link(i2c_list, link);
			if (i2c->buffer)
				free(i2c->buffer);
			free(i2c);
			g_list_free(link);
			return 0;
		}
		link = g_list_next(link);
	}

	return -1;
}

int peripheral_bus_i2c_open(int bus, int address, pb_i2c_data_h *i2c, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_i2c_data_h i2c_handle;
	int ret;
	int fd;

	if (peripheral_bus_i2c_data_get(bus, address, &pb_data->i2c_list)) {
		_E("Resource is in use, bus : %d, address : %d", bus, address);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = i2c_open(bus, &fd)) < 0)
		return ret;

	if ((ret = i2c_set_address(fd, address)) < 0) {
		i2c_close(fd);
		return ret;
	}

	i2c_handle = peripheral_bus_i2c_data_new(&pb_data->i2c_list);

	i2c_handle->fd = fd;
	i2c_handle->bus = bus;
	i2c_handle->address = address;
	i2c_handle->buffer = malloc(INITIAL_BUFFER_SIZE);

	if (!(i2c_handle->buffer)) {
		i2c_close(fd);
		_E("Failed to allocate data buffer");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}
	i2c_handle->buffer_size = INITIAL_BUFFER_SIZE;

	*i2c = i2c_handle;

	_D("bus : %d, address : %d, pgid = %d, id = %s", bus, address,
		i2c_handle->client_info.pgid,
		i2c_handle->client_info.id);

	return ret;
}

int peripheral_bus_i2c_close(pb_i2c_data_h i2c, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	int ret;

	_D("bus : %d, address : %d, pgid = %d", i2c->bus, i2c->address, i2c->client_info.pgid);

	if ((ret = i2c_close(i2c->fd)) < 0)
		return ret;

	if (peripheral_bus_i2c_data_free(i2c, &pb_data->i2c_list) < 0)
		_E("Failed to free i2c data");

	return ret;
}

int peripheral_bus_i2c_read(pb_i2c_data_h i2c, int length, GVariant **data_array)
{
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
	*data_array = peripheral_bus_build_variant_ay(i2c->buffer, length);

	return ret;

out:
	*data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	return ret;
}

int peripheral_bus_i2c_write(pb_i2c_data_h i2c, int length, GVariant *data_array)
{
	GVariantIter *iter;
	guchar str;
	int i = 0;

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

	return i2c_write(i2c->fd, i2c->buffer, length);
}
