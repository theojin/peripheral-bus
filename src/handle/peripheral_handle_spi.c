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

int peripheral_handle_spi_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_interface_spi_fd_close(handle->type.spi.fd);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to spi close fd");

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to free spi handle");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_handle_spi_create(int bus, int cs, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi bus");
	RETVM_IF(cs < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi cs");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi handle");

	int ret = PERIPHERAL_ERROR_NONE;

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h spi_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_spi_is_creatable(bus, cs, info);
	if (is_handle_creatable == false) {
		_E("spi %d.%d is not available", bus, cs);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	spi_handle = peripheral_handle_new(&info->spi_list);
	if (spi_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	spi_handle->list = &info->spi_list;
	spi_handle->type.spi.bus = bus;
	spi_handle->type.spi.cs = cs;
	spi_handle->type.spi.fd = -1;

	ret = peripheral_interface_spi_fd_open(bus, cs, &spi_handle->type.spi.fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to spi fd open");
		goto out;
	}

	*handle = spi_handle;
	return PERIPHERAL_ERROR_NONE;

out:
	peripheral_handle_spi_destroy(spi_handle);
	return ret;
}
