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

#include "peripheral_interface_uart.h"
#include "peripheral_handle_common.h"

#define INITIAL_BUFFER_SIZE 128
#define MAX_BUFFER_SIZE 8192

static bool __peripheral_bus_uart_is_available(int port, peripheral_bus_s *pb_data)
{
	pb_board_dev_s *uart = NULL;
	pb_data_h handle;
	GList *link;

	RETV_IF(pb_data == NULL, false);
	RETV_IF(pb_data->board == NULL, false);

	uart = peripheral_bus_board_find_device(PB_BOARD_DEV_UART, pb_data->board, port);
	if (uart == NULL) {
		_E("Not supported UART port : %d", port);
		return false;
	}

	link = pb_data->uart_list;
	while (link) {
		handle = (pb_data_h)link->data;
		if (handle->dev.uart.port == port) {
			_E("Resource is in use, port : %d", port);
			return false;
		}
		link = g_list_next(link);
	}

	return true;
}

int peripheral_bus_uart_open(int port, pb_data_h *handle, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_data_h uart_handle;
	int ret = PERIPHERAL_ERROR_NONE;
	int fd;

	if (!__peripheral_bus_uart_is_available(port, pb_data)) {
		_E("uart %d is not available", port);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = uart_open(port, &fd)) < 0) {
		_E("uart_open error (%d)", ret);
		goto open_err;
	}

	uart_handle = peripheral_bus_data_new(&pb_data->uart_list);
	if (!uart_handle) {
		_E("peripheral_bus_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	uart_handle->type = PERIPHERAL_BUS_TYPE_UART;
	uart_handle->list = &pb_data->uart_list;
	uart_handle->dev.uart.fd = fd;
	uart_handle->dev.uart.port = port;

	*handle = uart_handle;

	return PERIPHERAL_ERROR_NONE;

err:
	uart_close(fd);

open_err:
	return ret;
}

int peripheral_bus_uart_close(pb_data_h handle)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;
	int ret = PERIPHERAL_ERROR_NONE;

	_D("uart_close, port : %d, id = %s", uart->port, handle->client_info.id);

	if ((ret = uart_close(uart->fd)) < 0)
		return ret;

	if (peripheral_bus_data_free(handle) < 0) {
		_E("Failed to free uart data");
		ret = PERIPHERAL_ERROR_UNKNOWN;
	}

	return ret;
}