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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include "peripheral_interface_gpio.h"
#include "peripheral_common.h"

#define MAX_ERR_LEN 255

int gpio_open(int gpiopin)
{
	int fd, len, status;
	char gpio_export[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/export :%s\n", errmsg);
		return -ENXIO;
	}

	len = snprintf(gpio_export, GPIO_BUFFER_MAX, "%d", gpiopin);
	status = write(fd, gpio_export, len);

	if (status != len) {
		close(fd);
		_E("Error: gpio open error \n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_close(int gpiopin)
{
	int fd, len, status;
	char gpio_unexport[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/unexport %s\n", errmsg);
		return -ENXIO;
	}

	len = snprintf(gpio_unexport, GPIO_BUFFER_MAX, "%d", gpiopin);
	status = write(fd, gpio_unexport, len);

	if (status != len) {
		close(fd);
		_E("Error: gpio close error \n");
		return -EIO;
	}

	close(fd);

	return 0;
}
