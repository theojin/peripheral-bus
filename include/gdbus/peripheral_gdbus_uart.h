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

#ifndef __PERIPHERAL_GDBUS_UART_H__
#define __PERIPHERAL_GDBUS_UART_H__

#include "peripheral_io_gdbus.h"

gboolean peripheral_gdbus_uart_open(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		GUnixFDList *fd_list,
		gint port,
		gpointer user_data);

gboolean peripheral_gdbus_uart_close(
		PeripheralIoGdbusUart *uart,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

#endif /* __PERIPHERAL_GDBUS_UART_H__ */
