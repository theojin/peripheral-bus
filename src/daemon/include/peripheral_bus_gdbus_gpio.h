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

#ifndef __PERIPHERAL_BUS_GDBUS_GPIO_H__
#define __PERIPHERAL_BUS_GDBUS_GPIO_H__

#include "peripheral_io_gdbus.h"

gboolean handle_gpio_open(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint pin,
		gpointer user_data);

gboolean handle_gpio_close(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_gpio_get_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_gpio_set_direction(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint direction,
		gpointer user_data);

gboolean handle_gpio_read(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_gpio_write(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint value,
		gpointer user_data);

gboolean handle_gpio_get_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_gpio_set_edge_mode(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint edge,
		gpointer user_data);

gboolean handle_gpio_register_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_gpio_unregister_irq(
		PeripheralIoGdbusGpio *gpio,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

void peripheral_bus_emit_gpio_changed(
		PeripheralIoGdbusGpio *gpio,
		gint pin,
		gint value,
		guint64 timestamp);

#endif /* __PERIPHERAL_BUS_GDBUS_GPIO_H__ */
