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
#include <peripheral_internal.h>

#include "i2c.h"
#include "peripheral_common.h"

int peripheral_bus_i2c_open(peripheral_i2c_h i2c, int bus, int address)
{
	int ret;

	if ((ret = i2c_open(bus, &i2c->fd)) < 0)
		return ret;

	return i2c_set_address(i2c->fd, address);
}

int peripheral_bus_i2c_read(peripheral_i2c_h i2c, int length, unsigned char * data)
{
	return i2c_read(i2c->fd, data, length);
}

int peripheral_bus_i2c_write(peripheral_i2c_h i2c, int length, unsigned char * data)
{
	return i2c_write(i2c->fd, data, length);
}

int peripheral_bus_i2c_close(peripheral_i2c_h i2c)
{
	return i2c_close(i2c->fd);
}
