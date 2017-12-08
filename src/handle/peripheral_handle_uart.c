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

static bool __peripheral_handle_uart_is_creatable(int port, peripheral_info_s *info)
{
	pb_board_dev_s *uart = NULL;
	peripheral_h handle;
	GList *link;

	RETV_IF(info == NULL, false);
	RETV_IF(info->board == NULL, false);

	uart = peripheral_bus_board_find_device(PB_BOARD_DEV_UART, info->board, port);
	if (uart == NULL) {
		_E("Not supported UART port : %d", port);
		return false;
	}

	link = info->uart_list;
	while (link) {
		handle = (peripheral_h)link->data;
		if (handle->type.uart.port == port) {
			_E("Resource is in use, port : %d", port);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_handle_uart_destroy(peripheral_h handle)
{
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid uart handle");

	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_handle_free(handle);
	if (ret != PERIPHERAL_ERROR_NONE)
		_E("Failed to free uart handle");

	return ret;
}

int peripheral_handle_uart_create(int port, peripheral_h *handle, gpointer user_data)
{
	RETVM_IF(port < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid uart port");
	RETVM_IF(handle == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid uart handle");

	peripheral_info_s *info = (peripheral_info_s*)user_data;

	peripheral_h uart_handle = NULL;
	bool is_handle_creatable = false;

	is_handle_creatable = __peripheral_handle_uart_is_creatable(port, info);
	if (is_handle_creatable == false) {
		_E("uart %d is not available", port);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	uart_handle = peripheral_handle_new(&info->uart_list);
	if (uart_handle == NULL) {
		_E("peripheral_handle_new error");
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	uart_handle->list = &info->uart_list;
	uart_handle->type.uart.port = port;

	*handle = uart_handle;

	return PERIPHERAL_ERROR_NONE;
}
