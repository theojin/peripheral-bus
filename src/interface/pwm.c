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

#include "pwm.h"
#include "peripheral_common.h"

#define SYSFS_PWM_PATH	"/sys/class/pwm"

#define PATH_BUF_MAX	64
#define PWM_BUF_MAX	16
#define MAX_ERR_LEN	255

int pwm_open(int device, int channel)
{
	int fd, len, status;
	char pwm_dev[PATH_BUF_MAX] = {0};
	char pwm_buf[PWM_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/export", device);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", channel);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		_E("Failed to export pwmchip%d, pwm%", device, channel);
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_close(int device, int channel)
{
	int fd, len, status;
	char pwm_dev[PATH_BUF_MAX] = {0};
	char pwm_buf[PWM_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/unexport", device);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", channel);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		_E("Failed to unexport pwmchip%d, pwm%", device, channel);
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_set_period(int device, int channel, int period)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_E("Set period : %d, device : %d, channel : %d", period, device, channel);
	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", device, channel);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", period);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		close(fd);
		_E("Failed to set period, path : %s", pwm_dev);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_get_period(int device, int channel, int *period)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", device, channel);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	status = read(fd, pwm_buf, PWM_BUF_MAX);
	if (status < 0) {
		close(fd);
		_E("Failed to get period, path : %s", pwm_dev);
		return -EIO;
	}
	result = atoi(pwm_buf);
	*period = result;
	close(fd);

	return 0;
}

int pwm_set_duty_cycle(int device, int channel, int duty_cycle)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_E("Set duty_cycle : %d, device : %d, channel : %d", duty_cycle, device, channel);
	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", device, channel);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", duty_cycle);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		close(fd);
		_E("Failed to set duty cycle, path : %s", pwm_dev);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_get_duty_cycle(int device, int channel, int *duty_cycle)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", device, channel);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	status = read(fd, pwm_buf, PWM_BUF_MAX);
	if (status < 0) {
		close(fd);
		_E("Failed to get duty cycle, path : %s", pwm_dev);
		return -EIO;
	}
	result = atoi(pwm_buf);
	*duty_cycle = result;
	close(fd);

	return 0;
}

int pwm_set_enabled(int device, int channel, int enable)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_E("Set enable : %d, device : %d, channel : %d", enable, device, channel);
	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", device, channel);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	len = snprintf(pwm_buf, sizeof(pwm_buf), "%d", enable);
	status = write(fd, pwm_buf, len);
	if (status < 0) {
		close(fd);
		_E("Failed to set enable, path : %s", pwm_dev);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_get_enabled(int device, int channel, int *enable)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", device, channel);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENODEV;
	}

	status = read(fd, pwm_buf, PWM_BUF_MAX);
	if (status < 0) {
		close(fd);
		_E("Failed to get enable, path : %s", pwm_dev);
		return -EIO;
	}
	result = atoi(pwm_buf);
	enable = &result;
	close(fd);

	return 0;
}

