/*
 * Copyright (c) 2016-2018 Samsung Electronics Co., Ltd.
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

#ifndef __PERIPHERAL_HANDLE_H__
#define __PERIPHERAL_HANDLE_H__

#include <gio/gio.h>

#include "peripheral_board.h"
#include "peripheral_io_gdbus.h"

typedef struct {
	pb_board_s *board;
	/* devices */
	GList *gpio_list;
	GList *i2c_list;
	GList *pwm_list;
	GList *adc_list;
	GList *uart_list;
	GList *spi_list;
	/* gdbus variable */
	GDBusConnection *connection;
	PeripheralIoGdbusGpio *gpio_skeleton;
	PeripheralIoGdbusI2c *i2c_skeleton;
	PeripheralIoGdbusPwm *pwm_skeleton;
	PeripheralIoGdbusAdc *adc_skeleton;
	PeripheralIoGdbusUart *uart_skeleton;
	PeripheralIoGdbusSpi *spi_skeleton;
} peripheral_info_s;

typedef struct {
	int pin;
} peripheral_handle_gpio_s;

typedef struct {
	int bus;
	int address;
} peripheral_handle_i2c_s;

typedef struct {
	int chip;
	int pin;
} peripheral_handle_pwm_s;

typedef struct {
	int device;
	int channel;
} peripheral_handle_adc_s;

typedef struct {
	int port;
} peripheral_handle_uart_s;

typedef struct {
	int bus;
	int cs;
} peripheral_handle_spi_s;

typedef struct {
	uint watch_id;
	GList **list;
	union {
		peripheral_handle_gpio_s gpio;
		peripheral_handle_i2c_s i2c;
		peripheral_handle_pwm_s pwm;
		peripheral_handle_adc_s adc;
		peripheral_handle_uart_s uart;
		peripheral_handle_spi_s spi;
	} type;
} peripheral_handle_s;

typedef peripheral_handle_s *peripheral_h;

#endif /* __PERIPHERAL_HANDLE_H__ */
