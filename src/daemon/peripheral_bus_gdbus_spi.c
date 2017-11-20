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

#include <peripheral_io.h>

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_bus_spi.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"
#include "peripheral_bus_gdbus_spi.h"

static void __spi_on_name_vanished(GDBusConnection *connection,
		const gchar     *name,
		gpointer         user_data)
{
	pb_data_h spi_handle = (pb_data_h)user_data;
	_D("appid [%s] vanished ", name);

	g_bus_unwatch_name(spi_handle->watch_id);
	peripheral_bus_spi_close(spi_handle);
}

gboolean handle_spi_open(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint bus,
		gint cs,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	pb_data_h spi_handle = NULL;

	if ((ret = peripheral_bus_spi_open(bus, cs, &spi_handle, user_data)) < PERIPHERAL_ERROR_NONE)
		goto out;

	if (peripheral_bus_get_client_info(invocation, pb_data, &spi_handle->client_info) < 0) {
		peripheral_bus_spi_close(spi_handle);
		ret = PERIPHERAL_ERROR_UNKNOWN;
		spi_handle = NULL;
		goto out;
	}

	spi_handle->watch_id = g_bus_watch_name(G_BUS_TYPE_SYSTEM ,
			spi_handle->client_info.id,
			G_BUS_NAME_WATCHER_FLAGS_NONE ,
			NULL,
			__spi_on_name_vanished,
			spi_handle,
			NULL);
	_D("bus : %d, cs : %d, id = %s", bus, cs, spi_handle->client_info.id);

out:
	peripheral_io_gdbus_spi_complete_open(spi, invocation, GPOINTER_TO_UINT(spi_handle), ret);

	return true;
}
