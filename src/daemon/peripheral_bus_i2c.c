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

#include "i2c.h"
#include "peripheral_common.h"

int peripheral_bus_i2c_init(peripheral_i2c_context_h dev, int bus)
{
	return i2c_open(bus, &dev->fd);
}

int peripheral_bus_i2c_set_freq(peripheral_i2c_context_h dev, int mode)
{
	return i2c_set_frequency(dev->fd, mode);
}

int peripheral_bus_i2c_set_addr(peripheral_i2c_context_h dev, int addr)
{
	return i2c_set_address(dev->fd, addr);
}

int peripheral_bus_i2c_read(peripheral_i2c_context_h dev, int length, unsigned char * data, int addr)
{
	return i2c_read(dev->fd, data, length, addr);
}

int peripheral_bus_i2c_write(peripheral_i2c_context_h dev, int length, unsigned char * data, int addr)
{
	return i2c_write(dev->fd, data, length, addr);
}

int peripheral_bus_i2c_stop(peripheral_i2c_context_h dev)
{
	return i2c_close(dev->fd);
}

int peripheral_bus_i2c_process(peripheral_i2c_context_h dev, char *func_name, int value, unsigned char *data, int addr)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (dev == NULL) {
		_E("dev null error");
		return PERIPHERAL_ERROR_INVALID_PARAMETER;
	}

	if (!g_strcmp0(func_name, "INIT"))
		ret = peripheral_bus_i2c_init(dev, value);
	else if (!g_strcmp0(func_name, "SET_FREQ"))
		ret = peripheral_bus_i2c_set_freq(dev, value);
	else if (!g_strcmp0(func_name, "SET_ADDR"))
		ret = peripheral_bus_i2c_set_addr(dev, value);
	else if (!g_strcmp0(func_name, "READ"))
		ret = peripheral_bus_i2c_read(dev, value, data, addr);
	else if (!g_strcmp0(func_name, "WRITE"))
		ret = peripheral_bus_i2c_write(dev, value, data, addr);
	else if (!g_strcmp0(func_name, "STOP"))
		ret = peripheral_bus_i2c_stop(dev);

	return ret;
}
