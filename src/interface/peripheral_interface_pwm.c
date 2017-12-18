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

#include <stdlib.h>
#include "peripheral_interface_pwm.h"
#include "peripheral_interface_common.h"

int peripheral_interface_pwm_export(int chip, int pin)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");

	int ret;
	int fd;
	int length;
	char path[MAX_BUF_LEN] = {0, };
	char buf[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/export", chip);
	fd = open(path, O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	snprintf(buf, MAX_BUF_LEN, "chsmack -a \"*\" /sys/class/pwm/pwmchip%d/pwm%d/period", chip, pin);
	ret = system(buf);
	if (ret != 0) {
		_E("Failed to change period security label to read/write.");
		return PERIPHERAL_ERROR_IO_ERROR;
	}

	snprintf(buf, MAX_BUF_LEN, "chsmack -a \"*\" /sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip, pin);
	ret = system(buf);
	if (ret != 0) {
		_E("Failed to change duty_cycle security label to read/write.");
		return PERIPHERAL_ERROR_IO_ERROR;
	}

	snprintf(buf, MAX_BUF_LEN, "chsmack -a \"*\" /sys/class/pwm/pwmchip%d/pwm%d/polarity", chip, pin);
	ret = system(buf);
	if (ret != 0) {
		_E("Failed to change polarity security label to read/write.");
		return PERIPHERAL_ERROR_IO_ERROR;
	}

	snprintf(buf, MAX_BUF_LEN, "chsmack -a \"*\" /sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pin);
	ret = system(buf);
	if (ret != 0) {
		_E("Failed to change enable security label to read/write.");
		return PERIPHERAL_ERROR_IO_ERROR;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_pwm_unexport(int chip, int pin)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");

	int ret;
	int fd;
	int length;
	char path[MAX_BUF_LEN] = {0};
	char buf[MAX_BUF_LEN] = {0};

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/unexport", chip);
	fd = open(path, O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

static int __peripheral_interface_pwm_fd_period_open(int chip, int pin, int *fd_out)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for pwm period");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/pwm%d/period", chip, pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

static int __peripheral_interface_pwm_fd_duty_cycle_open(int chip, int pin, int *fd_out)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for pwm duty cycle");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chip, pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

static int __peripheral_interface_pwm_fd_polarity_open(int chip, int pin, int *fd_out)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for pwm polarity");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", chip, pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

static int __peripheral_interface_pwm_fd_enable_open(int chip, int pin, int *fd_out)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for pwm enable");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chip, pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_pwm_fd_list_create(int chip, int pin, GUnixFDList **list_out)
{
	RETVM_IF(chip < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm chip");
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid pwm pin");

	int ret;

	GUnixFDList *list = NULL;
	int fd_period = -1;
	int fd_duty_cycle = -1;
	int fd_polarity = -1;
	int fd_enable = -1;

	ret = __peripheral_interface_pwm_fd_period_open(chip, pin, &fd_period);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open pwm period fd");
		goto out;
	}

	ret = __peripheral_interface_pwm_fd_duty_cycle_open(chip, pin, &fd_duty_cycle);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open pwm duty cycle fd");
		goto out;
	}

	ret = __peripheral_interface_pwm_fd_polarity_open(chip, pin, &fd_polarity);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open pwm polarity fd");
		goto out;
	}

	ret = __peripheral_interface_pwm_fd_enable_open(chip, pin, &fd_enable);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open pwm enable fd");
		goto out;
	}

	list = g_unix_fd_list_new();
	if (list == NULL) {
		_E("Failed to create pwm fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(list, fd_period, NULL);
	g_unix_fd_list_append(list, fd_duty_cycle, NULL);
	g_unix_fd_list_append(list, fd_polarity, NULL);
	g_unix_fd_list_append(list, fd_enable, NULL);

	*list_out = list;

out:
	close(fd_period);
	close(fd_duty_cycle);
	close(fd_polarity);
	close(fd_enable);

	return ret;
}

void peripheral_interface_pwm_fd_list_destroy(GUnixFDList *list)
{
	if (list != NULL)
		g_object_unref(list); // file descriptors in list is closed in hear.
}