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

#include "pwm.h"
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

int pwm_set_period(int chip, int pin, int period)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_D("chip : %d, pin : %d, period : %d", chip, pin, period);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", chip, pin);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
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

int pwm_get_period(int chip, int pin, int *period)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", chip, pin);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
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

int pwm_set_duty_cycle(int chip, int pin, int duty_cycle)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_D("chip : %d, pin : %d, duty_cycle : %d", chip, pin, duty_cycle);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", chip, pin);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
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

int pwm_get_duty_cycle(int chip, int pin, int *duty_cycle)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", chip, pin);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
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

int pwm_set_polarity(int chip, int pin, pwm_polarity_e polarity)
{
	int fd, status;
	char pwm_dev[PATH_BUF_MAX] = {0};

	_D("chip : %d, pin : %d, polarity : %d", chip, pin, polarity);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/polarity", chip, pin);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
	}

	if (polarity == PWM_POLARITY_NORMAL)
		status = write(fd, "normal", strlen("normal")+1);
	else if (polarity == PWM_POLARITY_INVERSED)
		status = write(fd, "inversed", strlen("inversed")+1);
	else {
		_E("Invalid pwm polarity : %d", polarity);
		close(fd);
		return -EINVAL;
	}

	if (status <= 0) {
		close(fd);
		_E("Failed to set polarity, path : %s", pwm_dev);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_get_polarity(int chip, int pin, pwm_polarity_e *polarity)
{
	int fd, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/polarity", chip, pin);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
	}

	status = read(fd, pwm_buf, PWM_BUF_MAX);
	if (status < 0) {
		_E("Failed to get polarity, path : %s", pwm_dev);
		close(fd);
		return -EIO;
	}

	if (0 == strncmp(pwm_buf, "normal", strlen("normal")))
		*polarity = PWM_POLARITY_NORMAL;
	else if (0 == strncmp(pwm_buf, "inversed", strlen("inversed")))
		*polarity = PWM_POLARITY_INVERSED;
	else {
		close(fd);
		_E("Invalid pwm polarity : %d", pwm_buf);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_set_enable(int chip, int pin, bool enable)
{
	int fd, len, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	_D("chip : %d, pin : %d, enable : %d", chip, pin, enable);

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", chip, pin);
	fd = open(pwm_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
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

int pwm_get_enable(int chip, int pin, bool *enable)
{
	int fd, result, status;
	char pwm_buf[PWM_BUF_MAX] = {0};
	char pwm_dev[PATH_BUF_MAX] = {0};

	snprintf(pwm_dev, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", chip, pin);
	fd = open(pwm_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", pwm_dev, errmsg);
		return -ENXIO;
	}

	status = read(fd, pwm_buf, PWM_BUF_MAX);
	if (status < 0) {
		close(fd);
		_E("Failed to get enable, path : %s", pwm_dev);
		return -EIO;
	}
	result = atoi(pwm_buf);
	*enable = !!result;
	close(fd);

	return 0;
}

