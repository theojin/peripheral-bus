/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef __PERIPHERAL_BOARD_H__
#define __PERIPHERAL_BOARD_H__

#define BOARD_DEVICE_TREE	"/proc/device-tree/model"
#define BOARD_PINS_MAX	4
#define BOARD_ARGS_MAX	2

typedef enum {
	PB_BOARD_ARTIK710 = 0,
	PB_BOARD_ARTIK530,
	PB_BOARD_ARTIK520,
	PB_BOARD_RP3_B,
	PB_BOARD_EAGLEYE,
	PB_BOARD_UNKNOWN,
} pb_board_type_e;

typedef enum {
	PB_BOARD_DEV_GPIO = 0,
	PB_BOARD_DEV_I2C,
	PB_BOARD_DEV_PWM,
	PB_BOARD_DEV_UART,
	PB_BOARD_DEV_SPI,
	PB_BOARD_DEV_MAX,
} pb_board_dev_e;

typedef struct {
	pb_board_type_e type;
	char *name;
	char *path;
} pb_board_type_s;

typedef struct {
	pb_board_dev_e dev_type;
	unsigned int pins[BOARD_PINS_MAX];
	unsigned int num_pins;
	unsigned int args[BOARD_ARGS_MAX];
} pb_board_dev_s;

typedef struct {
	pb_board_type_e type;
	pb_board_dev_s *dev;
	unsigned int num_dev;
} pb_board_s;

pb_board_dev_s *peripheral_bus_board_find_device(pb_board_dev_e dev_type, pb_board_s *board, int arg, ...);
pb_board_s *peripheral_bus_board_init(void);
void peripheral_bus_board_deinit(pb_board_s *board);

#endif /* __PERIPHERAL_BOARD_H__ */
