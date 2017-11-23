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

	_D("chip : %d, pin : %d", chip, pin);

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

int peripheral_interface_pwm_fd_period_open(int chip, int pin, int *fd_out)
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

int peripheral_interface_pwm_fd_period_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for pwm period");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_pwm_fd_duty_cycle_open(int chip, int pin, int *fd_out)
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

int peripheral_interface_pwm_fd_duty_cycle_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for pwm duty cycle");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_pwm_fd_polarity_open(int chip, int pin, int *fd_out)
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

int peripheral_interface_pwm_fd_polarity_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for pwm polarity");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_pwm_fd_enable_open(int chip, int pin, int *fd_out)
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

int peripheral_interface_pwm_fd_enable_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for pwm enable");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}