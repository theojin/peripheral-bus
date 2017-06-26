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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iniparser.h>

#include <peripheral_io.h>

#include "peripheral_bus_board.h"
#include "peripheral_common.h"

#define STR_BUF_MAX			255

#define BOARD_INI_ARTIK710_PATH	SYSCONFDIR"/peripheral-bus/pio_board_artik710.ini"
#define BOARD_INI_RP3_B_12_PATH	SYSCONFDIR"/peripheral-bus/pio_board_rp3_b_12.ini"
#define BOARD_INI_UNKNOWN_PATH	SYSCONFDIR"/peripheral-bus/pio_board_unknown.ini"

static const pb_board_type_s pb_board_type[] = {
	{PB_BOARD_ARTIK710, "artik710 raptor", BOARD_INI_ARTIK710_PATH},
	{PB_BOARD_ARTIK530, "artik530 raptor", BOARD_INI_ARTIK710_PATH},
	{PB_BOARD_RP3_B_12, "Raspberry Pi 3 Model B Rev 1.2", BOARD_INI_RP3_B_12_PATH},
	{PB_BOARD_UNKOWN, "unkown board", BOARD_INI_UNKNOWN_PATH},
};

static int peripheral_bus_board_get_device_type(char *string)
{
	if (0 == strncmp(string, "gpio", strlen("gpio")))
		return PB_BOARD_DEV_GPIO;
	else if (0 == strncmp(string, "i2c", strlen("i2c")))
		return PB_BOARD_DEV_I2C;
	else if (0 == strncmp(string, "pwm", strlen("pwm")))
		return PB_BOARD_DEV_PWM;
	else if (0 == strncmp(string, "adc", strlen("adc")))
		return PB_BOARD_DEV_ADC;
	else if (0 == strncmp(string, "uart", strlen("uart")))
		return PB_BOARD_DEV_UART;
	else if (0 == strncmp(string, "spi", strlen("spi")))
		return PB_BOARD_DEV_SPI;

	return -1;
}

static void peripheral_bus_board_ini_parse_key(pb_board_dev_e type, char *string, unsigned int *args)
{
	switch (type) {
	case PB_BOARD_DEV_GPIO:
		sscanf(string, "%*50[^0-9]%d", &args[0]);
		break;
	case PB_BOARD_DEV_I2C:
		sscanf(string, "%*50[^-]-%d", &args[0]);
		break;
	case PB_BOARD_DEV_PWM:
		sscanf(string, "%*50[^0-9]%d%*50[^0-9]%d", &args[0], &args[1]);
		break;
	case PB_BOARD_DEV_ADC:
		sscanf(string, "%*50[^0-9]%d%*50[^0-9]%d", &args[0], &args[1]);
		break;
	case PB_BOARD_DEV_UART:
		sscanf(string, "%*50[^0-9]%d", &args[0]);
		break;
	case PB_BOARD_DEV_SPI:
		sscanf(string, "%*50[^0-9]%d.%d", &args[0], &args[1]);
		break;
	default:
		break;
	}
}

static int peripheral_bus_board_ini_parse_pins(char *string, unsigned int *pins)
{
	const char delimiter[] = ", ";
	int cnt_pins = 0;
	char *token, *ptr = NULL;

	if (string == NULL) return 0;

	token = strtok_r(string, delimiter, &ptr);
	while (token) {
		if (cnt_pins >= BOARD_PINS_MAX) break;

		pins[cnt_pins++] = atoi(token);
		token = strtok_r(NULL, delimiter, &ptr);
	}

	return cnt_pins;
}

static int peripheral_bus_board_ini_get_nkeys(dictionary *dict)
{
	int i, sec_num, key_num, ret = 0;
	char *section;

	sec_num = iniparser_getnsec(dict);
	for (i = 0; i < sec_num; i++) {
		section = iniparser_getsecname(dict, i);
		key_num = iniparser_getsecnkeys(dict, section);
		if (key_num <= 0) continue;
		ret += key_num;
	}

	return ret;
}

static int peripheral_bus_board_get_type(void)
{
	int fd, i;
	char str_buf[STR_BUF_MAX] = {0};
	int type = PB_BOARD_UNKOWN;

	fd = open(BOARD_DEVICE_TREE, O_RDONLY);
	if (fd < 0) {
		strerror_r(errno, str_buf, STR_BUF_MAX);
		_E("Cannot open %s, errmsg : %s", BOARD_DEVICE_TREE, str_buf);
		return -ENXIO;
	}

	if (read(fd, str_buf, STR_BUF_MAX) < 0) {
		_E("Failed to read model information, path : %s", BOARD_DEVICE_TREE);
		close(fd);
		return -EIO;
	}

	for (i = 0; i < PB_BOARD_UNKOWN; i++) {
		if (strstr(str_buf, pb_board_type[i].name)) {
			type = pb_board_type[i].type;
			break;
		}
	}

	close(fd);

	return type;
}

static pb_board_s *peripheral_bus_board_get_info()
{
	dictionary *dict = NULL;
	int i, j, ret;
	int sec_num, key_num, cnt_key = 0;
	char *section, *key_str;
	char **key_list = NULL;
	pb_board_dev_s *dev;
	pb_board_dev_e enum_dev;
	pb_board_s *board;

	board = (pb_board_s*)calloc(1, sizeof(pb_board_s));
	if (board == NULL) {
		_E("Failed to allocate pb_board_s");
		return NULL;
	}

	ret = peripheral_bus_board_get_type();
	if (ret < 0) {
		_E("Failed to get board type");
		goto err_get_type;
	}

	board->type = (pb_board_type_e)ret;
	dict = iniparser_load(pb_board_type[board->type].path);
	if (dict == NULL) {
		_E("Failed to load %s", pb_board_type[board->type].path);
		goto err_get_type;
	}

	board->num_dev = peripheral_bus_board_ini_get_nkeys(dict);
	if (board->num_dev == 0) {
		_E("There is no device to open");
		goto err_get_nkeys;
	}

	board->dev = calloc(board->num_dev, sizeof(pb_board_dev_s));
	if (board->dev == NULL) {
		_E("Failed to allocate pb_board_dev_s");
		goto err_get_nkeys;
	}

	sec_num = iniparser_getnsec(dict);
	for (i = 0; i < sec_num; i++) {
		section = iniparser_getsecname(dict, i);
		ret = peripheral_bus_board_get_device_type(section);
		if (ret < 0) continue;

		enum_dev = (pb_board_dev_e)ret;
		key_list = iniparser_getseckeys(dict, section);
		key_num = iniparser_getsecnkeys(dict, section);
		if (key_num <= 0) continue;

		for (j = 0; j < key_num; j++) {
			dev = &board->dev[cnt_key];
			dev->dev_type = enum_dev;
			key_str = iniparser_getstring(dict, key_list[j], NULL);
			peripheral_bus_board_ini_parse_key(dev->dev_type, key_list[j], dev->args);
			dev->num_pins = peripheral_bus_board_ini_parse_pins(key_str, dev->pins);
			cnt_key++;
		}
	}

	iniparser_freedict(dict);

	return board;

err_get_nkeys:
	iniparser_freedict(dict);

err_get_type:
	free(board);
	return NULL;
}

pb_board_dev_s *peripheral_bus_board_find_device(pb_board_dev_e dev_type, pb_board_s *board, int arg, ...)
{
	int i, args[2] = {0, };
	va_list ap;

	RETV_IF(board == NULL, false);

	args[0] = arg;
	if (dev_type == PB_BOARD_DEV_PWM || dev_type == PB_BOARD_DEV_ADC || dev_type == PB_BOARD_DEV_SPI) {
		va_start(ap, arg);
		args[1] = va_arg(ap, int);
		va_end(ap);
	}

	for (i = 0; i < board->num_dev; i++) {
		if (board->dev[i].dev_type != dev_type) continue;

		if (board->dev[i].args[0] == args[0] && board->dev[i].args[1] == args[1])
			return &board->dev[i];
	}

	return NULL;
}

pb_board_s *peripheral_bus_board_init(void)
{
	pb_board_s *board;

	board = peripheral_bus_board_get_info();

	return board;
}

void peripheral_bus_board_deinit(pb_board_s *board)
{
	if (board) {
		if (board->dev)
			free(board->dev);

		free(board);
	}
}
