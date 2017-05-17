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

#include "peripheral_io_gdbus.h"

typedef struct {
	/* daemon variable */
	/* devices */
	GList *gpio_list;
	GList *i2c_list;
	GList *pwm_list;
	GList *uart_list;
	/* gdbus variable */
	guint reg_id;
	GDBusConnection *connection;
	PeripheralIoGdbusGpio *gpio_skeleton;
	PeripheralIoGdbusI2c *i2c_skeleton;
	PeripheralIoGdbusPwm *pwm_skeleton;
	PeripheralIoGdbusUart *uart_skeleton;
} peripheral_bus_s;

typedef struct {
	char *id;
	pid_t pid;
	pid_t pgid;
} pb_client_info_s;

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

typedef struct {
	/* i2c device information */
	int bus;
	int address;
	int fd;
	/* data buffer */
	uint8_t *buffer;
	int buffer_size;
	/* client info */
	pb_client_info_s *client_info;
} peripheral_bus_i2c_data_s;

typedef struct {
	int device;
	int channel;
	/* client info */
	pb_client_info_s client_info;
} peripheral_bus_pwm_data_s;

typedef struct {
	int port;
	int fd;
	uint8_t *buffer;
	int buffer_size;
	/* client info */
	pb_client_info_s client_info;
} peripheral_bus_uart_data_s;

typedef peripheral_bus_gpio_data_s *pb_gpio_data_h;
typedef peripheral_bus_i2c_data_s *pb_i2c_data_h;
typedef peripheral_bus_pwm_data_s *pb_pwm_data_h;
typedef peripheral_bus_uart_data_s *pb_uart_data_h;

void peripheral_bus_emit_gpio_changed(PeripheralIoGdbusGpio *gpio,
									gint pin,
									gint state);
#endif /* __PERIPHERAL_BUS_H__ */
