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

static pb_uart_data_h peripheral_bus_uart_data_get(int port, GList **list)
{
	GList *uart_list = *list;
	GList *link;
	pb_uart_data_h uart_handle;

	link = uart_list;
	while (link) {
		uart_handle = (pb_uart_data_h)link->data;
		if (uart_handle->port == port)
			return uart_handle;
		link = g_list_next(link);
	}

	return NULL;
}

static pb_uart_data_h peripheral_bus_uart_data_new(GList **list)
{
	GList *uart_list = *list;
	pb_uart_data_h uart_handle;

	uart_handle = (pb_uart_data_h)calloc(1, sizeof(peripheral_bus_uart_data_s));
	if (uart_handle == NULL) {
		_E("failed to allocate peripheral_bus_uart_data_s");
		return NULL;
	}

	*list = g_list_append(uart_list, uart_handle);

	return uart_handle;
}

static int peripheral_bus_uart_data_free(pb_uart_data_h uart_handle, GList **uart_list)
{
	GList *link;

	RETVM_IF(uart_handle == NULL, -1, "handle is null");

	link = g_list_find(*uart_list, uart_handle);
	if (!link) {
		_E("handle does not exist in list");
		return -1;
	}

	*uart_list = g_list_remove_link(*uart_list, link);
	if (uart_handle->buffer)
		free(uart_handle->buffer);
	free(uart_handle);
	g_list_free(link);

	return 0;
}

int peripheral_bus_uart_open(int port, pb_uart_data_h *uart, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_uart_data_h uart_handle;
	int ret = PERIPHERAL_ERROR_NONE;
	int fd;

	if (peripheral_bus_uart_data_get(port, &pb_data->uart_list)) {
		_E("Resource is in use, port : %d", port);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = uart_open(port, &fd)) < 0)
		return ret;

	uart_handle = peripheral_bus_uart_data_new(&pb_data->uart_list);
	if (!uart_handle) {
		uart_close(fd);
		return PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	uart_handle->fd = fd;
	uart_handle->port = port;
	uart_handle->list = &pb_data->uart_list;

	uart_handle->buffer = (uint8_t*)calloc(1, INITIAL_BUFFER_SIZE);
	if (!uart_handle->buffer) {
		_E("Failed to allocate buffer");
		peripheral_bus_uart_data_free(uart_handle, &pb_data->uart_list);
		uart_close(fd);
		return  PERIPHERAL_ERROR_OUT_OF_MEMORY;
	}

	uart_handle->buffer_size = INITIAL_BUFFER_SIZE;
	*uart = uart_handle;

	return ret;
}

int peripheral_bus_uart_close(pb_uart_data_h uart)
{
	int ret;

	_D("uart_close, port : %d, id = %s", uart->port, uart->client_info.id);

	if ((ret = uart_close(uart->fd)) < 0)
		return ret;

	if (peripheral_bus_uart_data_free(uart, uart->list) < 0)
		_E("Failed to free uart data");

	return ret;
}

int peripheral_bus_uart_flush(pb_uart_data_h uart)
{
	return  uart_flush(uart->fd);
}

int peripheral_bus_uart_set_baudrate(pb_uart_data_h uart, int baudrate)
{
	return  uart_set_baudrate(uart->fd, baudrate);
}

int peripheral_bus_uart_set_mode(pb_uart_data_h uart, int byte_size, int parity, int stop_bits)
{
	return  uart_set_mode(uart->fd, byte_size, parity, stop_bits);
}

int peripheral_bus_uart_set_flowcontrol(pb_uart_data_h uart, bool xonxoff, bool rtscts)
{
	return  uart_set_flowcontrol(uart->fd, xonxoff, rtscts);
}

int peripheral_bus_uart_read(pb_uart_data_h uart, GVariant **data_array, int length)
{
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

int peripheral_bus_uart_write(pb_uart_data_h uart, GVariant *data_array, int length)
{
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
