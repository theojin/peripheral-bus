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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "peripheral_interface_pwm.h"
#include "peripheral_common.h"

#define SYSFS_PWM_PATH	"/sys/class/pwm"

#define PATH_BUF_MAX	64
#define PWM_BUF_MAX	16
#define MAX_ERR_LEN	255

int pwm_open(int chip, int pin)
{
	int fd, len, status;
	char pwm_dev[PATH_BUF_MAX] = {0};
	char pwm_buf[PWM_BUF_MAX] = {0};

	_D("chip : %d, pin : %d", chip, pin);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/export", chip);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", pin);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		_E("Failed to export pwmchip%d, pwm%d", chip, pin);
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_close(int chip, int pin)
{
	int fd, len, status;
	char pwm_dev[PATH_BUF_MAX] = {0};
	char pwm_buf[PWM_BUF_MAX] = {0};

	_D("chip : %d, pin : %d", chip, pin);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/unexport", chip);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", pin);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		_E("Failed to unexport pwmchip%d, pwm%", chip, pin);
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}
