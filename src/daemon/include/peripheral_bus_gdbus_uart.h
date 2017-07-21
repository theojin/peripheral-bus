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

#ifndef __PERIPHERAL_BUS_GDBUS_UART_H__
#define __PERIPHERAL_BUS_GDBUS_UART_H__

#include "peripheral_io_gdbus.h"

gboolean handle_uart_open(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint port,
		gpointer user_data);

gboolean handle_uart_close(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_uart_flush(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_uart_set_baudrate(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint baudrate,
		gpointer user_data);

gboolean handle_uart_set_mode(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		guint byte_size,
		guint parity,
		guint stop_bits,
		gpointer user_data);

gboolean handle_uart_set_flowcontrol(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gboolean xonxoff,
		gboolean rtscts,
		gpointer user_data);

gboolean handle_uart_read(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data);

gboolean handle_uart_write(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data);

#endif /* __PERIPHERAL_BUS_GDBUS_UART_H__ */
