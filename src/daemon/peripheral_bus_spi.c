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

static pb_spi_data_h peripheral_bus_spi_data_get(int bus, int cs, GList **list)
{
	GList *spi_list = *list;
	GList *link;
	pb_spi_data_h spi_handle;

	link = spi_list;
	while (link) {
		spi_handle = (pb_spi_data_h)link->data;
		if (spi_handle->bus == bus && spi_handle->cs == cs)
			return spi_handle;
		link = g_list_next(link);
	}

	return NULL;
}

static pb_spi_data_h peripheral_bus_spi_data_new(GList **list)
{
	GList *spi_list = *list;
	pb_spi_data_h spi_handle;

	spi_handle = (pb_spi_data_h)calloc(1, sizeof(peripheral_bus_spi_data_s));
	if (spi_handle == NULL) {
		_E("failed to allocate peripheral_bus_spi_data_s");
		return NULL;
	}

	*list = g_list_append(spi_list, spi_handle);

	return spi_handle;
}

static int peripheral_bus_spi_data_free(pb_spi_data_h spi_handle, GList **spi_list)
{
	GList *link;

	RETVM_IF(spi_handle == NULL, -1, "handle is null");

	link = g_list_find(*spi_list, spi_handle);
	if (!link) {
		_E("handle does not exist in list");
		return -1;
	}

	*spi_list = g_list_remove_link(*spi_list, link);
	if (spi_handle->rx_buf)
		free(spi_handle->rx_buf);
	if (spi_handle->tx_buf)
		free(spi_handle->tx_buf);
	free(spi_handle);
	g_list_free(link);

	return 0;
}

int peripheral_bus_spi_open(int bus, int cs, pb_spi_data_h *spi, gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	pb_spi_data_h spi_handle;
	int ret = PERIPHERAL_ERROR_NONE;
	int fd, bufsiz;

	if (peripheral_bus_spi_data_get(bus, cs, &pb_data->spi_list)) {
		_E("Resource is in use, bus : %d, cs : %d", bus, cs);
		return PERIPHERAL_ERROR_RESOURCE_BUSY;
	}

	if ((ret = spi_open(bus, cs, &fd)) < 0)
		goto err_open;

	spi_handle = peripheral_bus_spi_data_new(&pb_data->spi_list);
	if (!spi_handle) {
		_E("peripheral_bus_spi_data_new error");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_spi_data;
	}

	spi_handle->fd = fd;
	spi_handle->bus = bus;
	spi_handle->cs = cs;
	spi_handle->list = &pb_data->spi_list;

	if (!spi_get_buffer_size(&bufsiz)) {
		if (initial_buffer_size > bufsiz) initial_buffer_size = bufsiz;
		if (max_buffer_size > bufsiz) max_buffer_size = bufsiz;
	}

	spi_handle->rx_buf = (uint8_t*)calloc(1, initial_buffer_size);
	if (!spi_handle->rx_buf) {
		_E("Failed to allocate rx buffer");
		ret =  PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_rx_buf;
	}
	spi_handle->tx_buf = (uint8_t*)calloc(1, initial_buffer_size);
	if (!spi_handle->rx_buf) {
		_E("Failed to allocate tx buffer");
		ret =  PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto err_tx_buf;
	}

	spi_handle->rx_buf_size = initial_buffer_size;
	spi_handle->tx_buf_size = initial_buffer_size;
	*spi = spi_handle;

	return PERIPHERAL_ERROR_NONE;

err_tx_buf:
	if (spi_handle->rx_buf)
		free(spi_handle->rx_buf);
	spi_handle->rx_buf_size = 0;

err_rx_buf:
	peripheral_bus_spi_data_free(spi_handle, &pb_data->spi_list);

err_spi_data:
	spi_close(fd);

err_open:
	return ret;
}

int peripheral_bus_spi_close(pb_spi_data_h spi)
{
	int ret;

	if ((ret = spi_close(spi->fd)) < 0) {
		_E("spi_close error (%d)", ret);
		return ret;
	}

	if (peripheral_bus_spi_data_free(spi, spi->list) < 0) {
		_E("Failed to free spi data");
		return PERIPHERAL_ERROR_UNKNOWN;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_spi_set_mode(pb_spi_data_h spi, unsigned char mode)
{
	return spi_set_mode(spi->fd, mode);
}

int peripheral_bus_spi_get_mode(pb_spi_data_h spi, unsigned char *mode)
{
	return spi_get_mode(spi->fd, mode);
}

int peripheral_bus_spi_set_lsb_first(pb_spi_data_h spi, gboolean lsb)
{
	return spi_set_lsb_first(spi->fd, (unsigned char)lsb);
}

int peripheral_bus_spi_get_lsb_first(pb_spi_data_h spi, gboolean *lsb)
{
	int ret;
	unsigned char value;

	if ((ret = spi_get_lsb_first(spi->fd, &value)) < 0) {
		_E("spi_get_lsb_first error (%d)", ret);
		return ret;
	}
	*lsb = value ? true : false;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_spi_set_bits(pb_spi_data_h spi, unsigned char bits)
{
	return spi_set_bits(spi->fd, bits);
}

int peripheral_bus_spi_get_bits(pb_spi_data_h spi, unsigned char *bits)
{
	return spi_get_bits(spi->fd, bits);
}

int peripheral_bus_spi_set_frequency(pb_spi_data_h spi, unsigned int freq)
{
	return spi_set_frequency(spi->fd, freq);
}

int peripheral_bus_spi_get_frequency(pb_spi_data_h spi, unsigned int *freq)
{
	return spi_get_frequency(spi->fd, freq);
}

int peripheral_bus_spi_read(pb_spi_data_h spi, GVariant **data_array, int length)
{
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

int peripheral_bus_spi_write(pb_spi_data_h spi, GVariant *data_array, int length)
{
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

int peripheral_bus_spi_read_write(pb_spi_data_h spi, GVariant *tx_data_array, GVariant **rx_data_array, int length)
{
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

	ret = spi_read_write(spi->fd, spi->tx_buf, spi->rx_buf, length);
	*rx_data_array = peripheral_bus_build_variant_ay(spi->rx_buf, length);

	return ret;

out:
	*rx_data_array = peripheral_bus_build_variant_ay(err_buf, sizeof(err_buf));

	return ret;
}
