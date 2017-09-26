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

#ifndef __PERIPHERAL_BUS_GDBUS_SPI_H__
#define __PERIPHERAL_BUS_GDBUS_SPI_H__

#include "peripheral_io_gdbus.h"

gboolean handle_spi_open(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint bus,
		gint cs,
		gpointer user_data);

gboolean handle_spi_close(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_spi_set_mode(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar mode,
		gpointer user_data);

gboolean handle_spi_set_bit_order(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gboolean lsb,
		gpointer user_data);

gboolean handle_spi_set_bits_per_word(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar bits,
		gpointer user_data);

gboolean handle_spi_set_frequency(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint freq,
		gpointer user_data);

gboolean handle_spi_read(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data);

gboolean handle_spi_write(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data);

gboolean handle_spi_transfer(
		PeripheralIoGdbusSpi *spi,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *tx_data_array,
		gpointer user_data);

#endif /* __PERIPHERAL_BUS_GDBUS_SPI_H__ */
