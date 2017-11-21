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

#define SYSFS_GPIO_DIR "/sys/class/gpio"

int gpio_open(int gpiopin)
{
	int fd, len, status;
	char gpio_export[MAX_BUF_LEN] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	CHECK_ERROR(fd < 0);

	len = snprintf(gpio_export, MAX_BUF_LEN, "%d", gpiopin);
	status = write(fd, gpio_export, len);
	CHECK_ERROR(status != len);

	close(fd);

	return 0;
}

int gpio_close(int gpiopin)
{
	int fd, len, status;
	char gpio_unexport[MAX_BUF_LEN] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	CHECK_ERROR(fd < 0);

	len = snprintf(gpio_unexport, MAX_BUF_LEN, "%d", gpiopin);
	status = write(fd, gpio_unexport, len);
	CHECK_ERROR(status != len);

	close(fd);

	return 0;
}
