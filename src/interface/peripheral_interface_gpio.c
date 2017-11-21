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

#include "peripheral_interface_gpio.h"
#include "peripheral_interface_common.h"

int peripheral_interface_gpio_open(int pin)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");

	int ret;
	int fd;
	int length;
	char buf[MAX_BUF_LEN] = {0, };

	fd = open("/sys/class/gpio/export", O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_close(int pin)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");

	int ret;
	int fd;
	int length;
	char buf[MAX_BUF_LEN] = {0, };

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_open_file_direction(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio direction");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_open_file_edge(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio edge");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/edge", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_open_file_value(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio value");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}
