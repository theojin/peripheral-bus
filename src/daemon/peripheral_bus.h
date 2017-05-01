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

#ifndef __PERIPHERAL_BUS_H__
#define __PERIPHERAL_BUS_H__

#include <gio/gio.h>

typedef struct {
	/* daemon variable */
	/* devices */
	GList *gpio_list;
	/* gdbus variable */
	guint reg_id;
	GDBusConnection *connection;
	PeripheralIoGdbusGpio *gpio_skeleton;
	PeripheralIoGdbusI2c *i2c_skeleton;
	PeripheralIoGdbusPwm *pwm_skeleton;
} peripheral_bus_s;

typedef struct {
	/* gpio info */
	int pin;
	int direction;
	int edge;
	/* interrupt variable */
	int irq_en;
	int value_fd;
	GIOChannel *io;
	guint io_id;
	/* gdbus variable */
	PeripheralIoGdbusGpio *gpio_skeleton;
} peripheral_bus_gpio_data_s;

typedef peripheral_bus_gpio_data_s * pb_gpio_data_h;

void peripheral_bus_emit_gpio_changed(PeripheralIoGdbusGpio *gpio,
									gint pin,
									gint state);
#endif /* __PERIPHERAL_BUS_H__ */
