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
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "adc.h"
#include "peripheral_common.h"

#define SYSFS_ADC_PATH		"/sys/bus/iio/devices/iio:device"
#define SYSFS_ADC_PATH_OLD	"/sys/bus/iio/devices/device"

#define PATH_BUF_MAX		64
#define ADC_BUF_MAX		16
#define MAX_ERR_LEN		255

int adc_get_path(unsigned int device, char *path, int length)
{
	char adc_dev[PATH_BUF_MAX] = {0};

	snprintf(adc_dev, PATH_BUF_MAX, "%s%d", SYSFS_ADC_PATH, device);
	if (access(adc_dev, F_OK) == 0) {
		strncpy(path, SYSFS_ADC_PATH, length);
		return 0;
	} else {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", adc_dev, errmsg);
	}

	snprintf(adc_dev, PATH_BUF_MAX, "%s%d", SYSFS_ADC_PATH_OLD, device);
	if (access(adc_dev, F_OK) == 0) {
		strncpy(path, SYSFS_ADC_PATH_OLD, length);
		return 0;
	} else {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", adc_dev, errmsg);
	}

	return -ENXIO;
}

int adc_read(unsigned int device, unsigned int channel, char *path, int *data)
{
	int fd, result, status;
	char adc_buf[ADC_BUF_MAX] = {0};
	char adc_dev[PATH_BUF_MAX] = {0};

	snprintf(adc_dev, PATH_BUF_MAX, "%s%d%s%d%s", path, device, "/in_voltage", channel, "_raw");
	fd = open(adc_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", adc_dev, errmsg);
		return -ENXIO;
	}

	status = read(fd, adc_buf, ADC_BUF_MAX);
	if (status < 0) {
		close(fd);
		_E("Failed to get adc, path : %s", adc_dev);
		return -EIO;
	}
	result = atoi(adc_buf);
	*data = result;
	close(fd);

	return 0;
}
