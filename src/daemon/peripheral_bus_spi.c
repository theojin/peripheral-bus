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

#include "spi.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

static int initial_buffer_size = 128;
static int max_buffer_size = 4096;

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
	int fd, bufsiz;

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

	if (!spi_get_buffer_size(&bufsiz)) {
		if (initial_buffer_size > bufsiz) initial_buffer_size = bufsiz;
		if (max_buffer_size > bufsiz) max_buffer_size = bufsiz;
	}

	spi_handle->dev.spi.rx_buf = (uint8_t*)calloc(1, initial_buffer_size);
	if (!spi_handle->dev.spi.rx_buf) {
		_E("Failed to allocate rx buffer");
		ret =  PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_rx_buf;
	}
	spi_handle->dev.spi.tx_buf = (uint8_t*)calloc(1, initial_buffer_size);
	if (!spi_handle->dev.spi.tx_buf) {
		_E("Failed to allocate tx buffer");
		ret =  PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_tx_buf;
	}

	spi_handle->dev.spi.rx_buf_size = initial_buffer_size;
	spi_handle->dev.spi.tx_buf_size = initial_buffer_size;
	*handle = spi_handle;

	return PERIPHERAL_ERROR_NONE;

err_tx_buf:
	if (spi_handle->dev.spi.rx_buf)
		free(spi_handle->dev.spi.rx_buf);
	spi_handle->dev.spi.rx_buf_size = 0;

err_rx_buf:
	peripheral_bus_data_free(spi_handle);

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

int peripheral_bus_spi_set_mode(pb_data_h handle, unsigned char mode)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;

	return spi_set_mode(spi->fd, mode);
}

int peripheral_bus_spi_set_bit_order(pb_data_h handle, gboolean lsb)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;

	return spi_set_bit_order(spi->fd, (unsigned char)lsb);
}

int peripheral_bus_spi_set_bits_per_word(pb_data_h handle, unsigned char bits)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;

	return spi_set_bits_per_word(spi->fd, bits);
}

int peripheral_bus_spi_set_frequency(pb_data_h handle, unsigned int freq)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;

	return spi_set_frequency(spi->fd, freq);
}

int peripheral_bus_spi_read(pb_data_h handle, GVariant **data_array, int length)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;
	uint8_t err_buf[2] = {0, };
	int ret;

	/* Limit maximum length */
	if (length > max_buffer_size) length = max_buffer_size;

	/* Increase buffer if needed */
	if (length > spi->rx_buf_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(spi->rx_buf, length);
		if (!buffer) {
			ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
			_E("Failed to realloc buffer");
			goto out;
		}
		spi->rx_buf = buffer;
		spi->rx_buf_size = length;
	}

	ret = spi_read(spi->fd, spi->rx_buf, length);
	*data_array = peripheral_bus_build_variant_ay(spi->rx_buf, length);

	return ret;

out:
	*data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	return ret;
}

int peripheral_bus_spi_write(pb_data_h handle, GVariant *data_array, int length)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;
	GVariantIter *iter;
	guchar str;
	int i = 0;

	/* Limit maximum length */
	if (length > max_buffer_size) length = max_buffer_size;

	/* Increase buffer if needed */
	if (length > spi->tx_buf_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(spi->tx_buf, length);
		RETVM_IF(buffer == NULL, PERIPHERAL_ERROR_OUT_OF_MEMORY, "Failed to realloc tx buffer");

		spi->tx_buf = buffer;
		spi->tx_buf_size = length;
	}

	g_variant_get(data_array, "a(y)", &iter);
	while (g_variant_iter_loop(iter, "(y)", &str) && i < length)
		spi->tx_buf[i++] = str;
	g_variant_iter_free(iter);

	return spi_write(spi->fd, spi->tx_buf, length);
}

int peripheral_bus_spi_transfer(pb_data_h handle, GVariant *tx_data_array, GVariant **rx_data_array, int length)
{
	peripheral_bus_spi_s *spi = &handle->dev.spi;
	uint8_t err_buf[2] = {0, };
	GVariantIter *iter;
	guchar str;
	int i = 0;
	int ret;

	/* Limit maximum length */
	if (length > max_buffer_size) length = max_buffer_size;

	/* Increase rx buffer if needed */
	if (length > spi->rx_buf_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(spi->rx_buf, length);
		if (!buffer) {
			ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
			_E("Failed to realloc rx buffer");
			goto out;
		}
		spi->rx_buf = buffer;
		spi->rx_buf_size = length;
	}

	/* Increase tx buffer if needed */
	if (length > spi->tx_buf_size) {
		uint8_t *buffer;

		buffer = (uint8_t*)realloc(spi->tx_buf, length);
		if (!buffer) {
			ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
			_E("Failed to realloc tx buffer");
			goto out;
		}

		spi->tx_buf = buffer;
		spi->tx_buf_size = length;
	}
	g_variant_get(tx_data_array, "a(y)", &iter);
	while (g_variant_iter_loop(iter, "(y)", &str) && i < length)
		spi->tx_buf[i++] = str;
	g_variant_iter_free(iter);

	ret = spi_transfer(spi->fd, spi->tx_buf, spi->rx_buf, length);
	*rx_data_array = peripheral_bus_build_variant_ay(spi->rx_buf, length);

	return ret;

out:
	*rx_data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	return ret;
}
