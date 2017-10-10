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

#include "uart.h"
#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

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

	uart_handle->dev.uart.buffer = (uint8_t*)calloc(1, INITIAL_BUFFER_SIZE);
	if (!uart_handle->dev.uart.buffer) {
		_E("Failed to allocate buffer");
		peripheral_bus_data_free(uart_handle);
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err;
	}

	uart_handle->dev.uart.buffer_size = INITIAL_BUFFER_SIZE;
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

int peripheral_bus_uart_set_baud_rate(pb_data_h handle, int baud_rate)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;

	return uart_set_baud_rate(uart->fd, baud_rate);
}

int peripheral_bus_uart_set_byte_size(pb_data_h handle, int byte_size)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;

	return uart_set_byte_size(uart->fd, byte_size);
}

int peripheral_bus_uart_set_parity(pb_data_h handle, int parity)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;

	return uart_set_parity(uart->fd, parity);
}
int peripheral_bus_uart_set_stop_bits(pb_data_h handle, int stop_bits)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;

	return uart_set_stop_bits(uart->fd, stop_bits);
}

int peripheral_bus_uart_set_flow_control(pb_data_h handle, bool xonxoff, bool rtscts)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;

	return uart_set_flow_control(uart->fd, xonxoff, rtscts);
}

int peripheral_bus_uart_read(pb_data_h handle, GVariant **data_array, int length)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;
	int ret;

	/* Limit maximum length */
	if (length > MAX_BUFFER_SIZE) length = MAX_BUFFER_SIZE;

	/* Increase buffer if needed */
	if (length > uart->buffer_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(uart->buffer, length);
		RETVM_IF(buffer == NULL, PERIPHERAL_ERROR_OUT_OF_MEMORY, "Failed to realloc buffer");

		uart->buffer = buffer;
		uart->buffer_size = length;
	}

	ret = uart_read(uart->fd, uart->buffer, length);
	if (ret > 0)
		*data_array = peripheral_bus_build_variant_ay(uart->buffer, ret);

	return ret;
}

int peripheral_bus_uart_write(pb_data_h handle, GVariant *data_array, int length)
{
	peripheral_bus_uart_s *uart = &handle->dev.uart;
	GVariantIter *iter;
	guchar str;
	int i = 0;

	/* Limit maximum length */
	if (length > MAX_BUFFER_SIZE) length = MAX_BUFFER_SIZE;

	/* Increase buffer if needed */
	if (length > uart->buffer_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(uart->buffer, length);
		RETVM_IF(buffer == NULL, PERIPHERAL_ERROR_OUT_OF_MEMORY, "Failed to realloc buffer");

		uart->buffer = buffer;
		uart->buffer_size = length;
	}

	g_variant_get(data_array, "a(y)", &iter);
	while (g_variant_iter_loop(iter, "(y)", &str) && i < length)
		uart->buffer[i++] = str;
	g_variant_iter_free(iter);

	return uart_write(uart->fd, uart->buffer, length);
}
