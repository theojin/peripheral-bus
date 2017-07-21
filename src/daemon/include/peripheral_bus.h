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
#include "peripheral_bus_board.h"

typedef enum {
	PERIPHERAL_BUS_TYPE_GPIO = 0,
	PERIPHERAL_BUS_TYPE_I2C,
	PERIPHERAL_BUS_TYPE_PWM,
	PERIPHERAL_BUS_TYPE_ADC,
	PERIPHERAL_BUS_TYPE_UART,
	PERIPHERAL_BUS_TYPE_SPI,
} peripheral_bus_type_e;

typedef struct {
	pb_board_s *board;
	/* daemon variable */
	char *adc_path;
	/* devices */
	GList *gpio_list;
	GList *i2c_list;
	GList *pwm_list;
	GList *uart_list;
	GList *spi_list;
	/* gdbus variable */
	guint reg_id;
	GDBusConnection *connection;
	PeripheralIoGdbusGpio *gpio_skeleton;
	PeripheralIoGdbusI2c *i2c_skeleton;
	PeripheralIoGdbusPwm *pwm_skeleton;
	PeripheralIoGdbusAdc *adc_skeleton;
	PeripheralIoGdbusUart *uart_skeleton;
	PeripheralIoGdbusSpi *spi_skeleton;
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
} peripheral_bus_gpio_s;

typedef struct {
	/* i2c device information */
	int bus;
	int address;
	int fd;
	/* data buffer */
	uint8_t *buffer;
	int buffer_size;
} peripheral_bus_i2c_s;

typedef struct {
	int device;
	int channel;
} peripheral_bus_pwm_s;

typedef struct {
	int port;
	int fd;
	uint8_t *buffer;
	int buffer_size;
} peripheral_bus_uart_s;

typedef struct {
	int bus;
	int cs;
	int fd;
	/* data buffer */
	uint8_t *rx_buf;
	uint8_t *tx_buf;
	int rx_buf_size;
	int tx_buf_size;
} peripheral_bus_spi_s;

typedef struct {
	peripheral_bus_type_e type;
	uint watch_id;
	GList **list;
	/* client info */
	pb_client_info_s client_info;
	union {
		peripheral_bus_gpio_s gpio;
		peripheral_bus_i2c_s i2c;
		peripheral_bus_pwm_s pwm;
		peripheral_bus_uart_s uart;
		peripheral_bus_spi_s spi;
	} dev;
} peripheral_bus_data_s;

typedef peripheral_bus_data_s *pb_data_h;

#endif /* __PERIPHERAL_BUS_H__ */
