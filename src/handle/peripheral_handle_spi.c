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

static bool __peripheral_handle_spi_is_creatable(int bus, int cs, peripheral_info_s *info)
{
	pb_board_dev_s *spi = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	spi = peripheral_bus_board_find_device(PB_BOARD_DEV_SPI, info->board, bus, cs);
	if (spi == NULL) {
		_E("Not supported SPI bus : %d, cs : %d", bus, cs);
		return false;
	}

	link = info->spi_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.spi.bus == bus && handle->type.spi.cs == cs) {
			_E("Resource is in use, bus : %d, cs : %d", bus, cs);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_spi_create(int bus, int cs, peripheral_h *handle, gpointer user_data)
{
	peripheral_info_s *info = (peripheral_info_s*)user_data;
	peripheral_h spi_handle;
	int ret = PERIPHERAL_ERROR_NONE;

	if (!__peripheral_handle_spi_is_creatable(bus, cs, info)) {
		_E("spi %d.%d is not available", bus, cs);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	// TODO : make fd list using the interface function

	spi_handle = peripheral_handle_new(&info->spi_list);
	if (!spi_handle) {
		_E("peripheral_handle_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_spi_data;
	}

	spi_handle->list = &info->spi_list;
	spi_handle->type.spi.bus = bus;
	spi_handle->type.spi.cs = cs;

	*handle = spi_handle;

	return PERIPHERAL_ERROR_NONE;

err_spi_data:
	return ret;
}

int peripheral_handle_spi_destroy(peripheral_h handle)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (peripheral_handle_free(handle) < 0) {
		_E("Failed to free spi data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}