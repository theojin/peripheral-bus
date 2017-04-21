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

#include "gpio.h"
#include "peripheral_common.h"


int peripheral_bus_gpio_open(peripheral_gpio_h gpio)
{
	gpio_edge_e edge;
	gpio_direction_e direction;

	if (gpio_open(gpio->pin) < 0) {
		_E("gpio_open error");
		return PERIPHERAL_ERROR_INVALID_PARAMETER;
	}

	if (gpio_get_edge_mode(gpio->pin, (gpio_edge_e*)&edge) < 0) {
		gpio_close(gpio->pin);
		_E("gpio_get_edge_mode error");
		return PERIPHERAL_ERROR_INVALID_PARAMETER;
	}
	gpio->edge = edge;

	if (gpio_get_direction(gpio->pin, (gpio_direction_e*)&direction) < 0) {
		gpio_close(gpio->pin);
		_E("gpio_get_direction error");
		return PERIPHERAL_ERROR_INVALID_PARAMETER;
	}
	gpio->direction = direction;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_bus_gpio_set_direction(peripheral_gpio_h gpio)
{
	return gpio_set_direction(gpio->pin, (gpio_direction_e)gpio->direction);
}

int peripheral_bus_gpio_get_direction(peripheral_gpio_h gpio)
{
	return gpio_get_direction(gpio->pin, (gpio_direction_e*)&gpio->direction);
}

int peripheral_bus_gpio_set_edge(peripheral_gpio_h gpio)
{
	return gpio_set_edge_mode(gpio->pin, (gpio_edge_e)gpio->edge);
}

int peripheral_bus_gpio_get_edge(peripheral_gpio_h gpio)
{
	return gpio_get_edge_mode(gpio->pin, (gpio_edge_e*)&gpio->edge);
}

int peripheral_bus_gpio_write(peripheral_gpio_h gpio, int value)
{
	return gpio_write(gpio->pin, value);
}

int peripheral_bus_gpio_read(peripheral_gpio_h gpio, int *read_value)
{
	return gpio_read(gpio->pin, read_value);
}

int peripheral_bus_gpio_close(peripheral_gpio_h gpio)
{
	return gpio_close(gpio->pin);
}
