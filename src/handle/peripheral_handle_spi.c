/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "peripheral_interface_spi.h"
#include "peripheral_handle_common.h"

static bool __peripheral_bus_spi_is_available(int bus, int cs, peripheral_bus_s *pb_data)
{
	pb_board_dev_s *spi = NULL;
	pb_data_h handle;
	GList *link;

	RETV_IF(pb_data == NULL, false);
	RETV_IF(pb_data->board == NULL, false);

	spi = peripheral_bus_board_find_device(PB_BOARD_DEV_SPI, pb_data->board, bus, cs);
	if (spi == NULL) {
		_E("Not supported SPI bus : %d, cs : %d", bus, cs);
		return false;
	}

	link = pb_data->spi_list;
	while (link) {
		handle = (pb_data_h)link->data;
		if (handle->dev.spi.bus == bus && handle->dev.spi.cs == cs) {
			_E("Resource is in use, bus : %d, cs : %d", bus, cs);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_bus_spi_open(int bus, int cs, pb_data_h *handle, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h spi_handle;
	int ret = PERIPHERAL_ERROR_NONE;
	int fd;

	if (!__peripheral_bus_spi_is_available(bus, cs, pb_data)) {
		_E("spi %d.%d is not available", bus, cs);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = spi_open(bus, cs, &fd)) < 0) {
		_E("spi_open error (%d)", ret);
		goto err_open;
	}

	spi_handle = peripheral_bus_data_new(&pb_data->spi_list);
	if (!spi_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_spi_data;
	}

	spi_handle->type = PERIPHERAL_BUS_TYPE_SPI;
	spi_handle->list = &pb_data->spi_list;
	spi_handle->dev.spi.fd = fd;
	spi_handle->dev.spi.bus = bus;
	spi_handle->dev.spi.cs = cs;

	*handle = spi_handle;

	return PERIPHERAL_ERROR_NONE;

err_spi_data:
	spi_close(fd);

err_open:
	return ret;
}

int peripheral_bus_spi_close(pb_data_h handle)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;
	int ret = PERIPHERAL_ERROR_NONE;

	if ((ret = spi_close(spi->fd)) < 0)
		return ret;

	if (peripheral_bus_data_free(handle) < 0) {
		_E("Failed to free spi data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}