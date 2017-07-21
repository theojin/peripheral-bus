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

#ifndef __PERIPHERAL_BUS_GDBUS_I2C_H__
#define __PERIPHERAL_BUS_GDBUS_I2C_H__

#include "peripheral_io_gdbus.h"

gboolean handle_i2c_open(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint bus,
		gint address,
		gpointer user_data);

gboolean handle_i2c_close(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gpointer user_data);

gboolean handle_i2c_read(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		gpointer user_data);

gboolean handle_i2c_write(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		gint length,
		GVariant *data_array,
		gpointer user_data);

gboolean handle_i2c_smbus_ioctl(
		PeripheralIoGdbusI2c *i2c,
		GDBusMethodInvocation *invocation,
		gint handle,
		guchar read_write,
		guchar command,
		guint size,
		guint16 data_in,
		gpointer user_data);

#endif /* __PERIPHERAL_BUS_GDBUS_I2C_H__ */
