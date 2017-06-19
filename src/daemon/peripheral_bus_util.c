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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gio/gio.h>

#include "peripheral_bus.h"
#include "peripheral_common.h"

GVariant *peripheral_bus_build_variant_ay(uint8_t *data, int length)
{
	GVariantBuilder *builder;
	GVariant *variant;
	int i;

	if (data == NULL)
		return NULL;

	builder = g_variant_builder_new(G_VARIANT_TYPE("a(y)"));

	for (i = 0; i < length; i++)
		g_variant_builder_add(builder, "(y)", data[i]);

	variant = g_variant_new("a(y)", builder);
	g_variant_builder_unref(builder);

	return variant;
}

pb_data_h peripheral_bus_data_new(GList **plist)
{
	GList *list = *plist;
	pb_data_h handle;

	handle = (pb_data_h)calloc(1, sizeof(peripheral_bus_data_s));
	if (handle == NULL) {
		_E("failed to allocate peripheral_bus_data_s");
		return NULL;
	}

	*plist = g_list_append(list, handle);

	return handle;
}

int peripheral_bus_data_free(pb_data_h handle)
{
	GList *list = *handle->list;
	GList *link;

	RETVM_IF(handle == NULL, -1, "handle is null");

	link = g_list_find(list, handle);
	if (!link) {
		_E("handle does not exist in list");
		return -1;
	}

	*handle->list = g_list_remove_link(list, link);

	switch (handle->type) {
	case PERIPHERAL_BUS_TYPE_I2C:
		if (handle->dev.i2c.buffer)
			free(handle->dev.i2c.buffer);
		break;
	case PERIPHERAL_BUS_TYPE_UART:
		if (handle->dev.uart.buffer)
			free(handle->dev.uart.buffer);
		break;
	case PERIPHERAL_BUS_TYPE_SPI:
		if (handle->dev.spi.rx_buf)
			free(handle->dev.spi.rx_buf);
		if (handle->dev.spi.tx_buf)
			free(handle->dev.spi.tx_buf);
		break;
	default:
		break;
	}

	free(handle);
	g_list_free(link);

	return 0;
}
