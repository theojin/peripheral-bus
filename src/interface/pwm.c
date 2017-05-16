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
#define PWM_BUF_MAX		16

int pwm_open(int device, int channel)
{
	int fd, len, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	pwm_close(device, channel);

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/export", device);
	if ((fd = open(fName, O_WRONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	len = snprintf(buff, sizeof(buff), "%d", channel);
	if ((ret = write(fd, buff, len)) < 0) {
		_E("Error[%d]: can't write %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	close(fd);
	return 0;
}

int pwm_close(int device, int channel)
{
	int fd, len, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	pwm_set_enabled(device, channel, 0); // disable

	_D("[PWM] close!!\n");
	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/unexport", device);
	if ((fd = open(fName, O_WRONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	len = snprintf(buff, sizeof(buff), "%d", channel);

	if ((ret = write(fd, buff, len)) < 0) {
		_E("Error[%d]: can't write %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	close(fd);
	return 0;
}

int pwm_set_period(int device, int channel, int period)
{
	int fd, len, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", device, channel);
	if ((fd = open(fName, O_WRONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}
	_D("[PWM] period = %d\n", period);
	len = snprintf(buff, sizeof(buff), "%d", period);
	if ((ret = write(fd, buff, len)) < 0) {
		_E("Error[%d]: can't write %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	close(fd);
	return 0;
}

int pwm_get_period(int device, int channel, int *period)
{
	int fd, result, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/period", device, channel);
	if ((fd = open(fName, O_RDONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	if ((ret = read(fd, buff, PWM_BUF_MAX)) < 0) {
		_E("Error[%d]: can't read %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	result = atoi(buff);
	*period = result;
	close(fd);

	return 0;
}

int pwm_set_duty_cycle(int device, int channel, int duty_cycle)
{
	int fd, len, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", device, channel);
	if ((fd = open(fName, O_WRONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}
	_D("[PWM] duty_cycle = %d\n", duty_cycle);

	len = snprintf(buff, sizeof(buff), "%d", duty_cycle);
	if ((ret = write(fd, buff, len)) < 0) {
		_E("Error[%d]: can't write %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	close(fd);

	return 0;
}

int pwm_get_duty_cycle(int device, int channel, int *duty_cycle)
{
	int fd, result, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/duty_cycle", device, channel);
	if ((fd = open(fName, O_RDONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	if ((ret = read(fd, buff, PWM_BUF_MAX)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	result = atoi(buff);
	*duty_cycle = result;
	close(fd);

	return 0;
}

int pwm_set_enabled(int device, int channel, int enable)
{
	int fd, len, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	_D("[PWM] set enable = %d!!\n", enable);
	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", device, channel);
	if ((fd = open(fName, O_WRONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	len = snprintf(buff, sizeof(buff), "%d", enable);
	if ((ret = write(fd, buff, len)) < 0) {
		_E("Error[%d]: can't write %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	close(fd);
	return 0;
}

int pwm_get_enabled(int device, int channel, int *enable)
{
	int fd, result, ret;
	char buff[PWM_BUF_MAX] = {0};
	char fName[PATH_BUF_MAX] = {0};

	snprintf(fName, PATH_BUF_MAX, SYSFS_PWM_PATH "/pwmchip%d/pwm%d/enable", device, channel);
	if ((fd = open(fName, O_RDONLY)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENOENT;
	}

	if ((ret = read(fd, buff, PWM_BUF_MAX)) < 0) {
		_E("Error[%d]: can't read %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		close(fd);
		return -EIO;
	}
	result = atoi(buff);
	enable = &result;
	close(fd);

	return 0;
}
