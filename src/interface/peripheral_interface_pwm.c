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

#define SYSFS_PWM_PATH	"/sys/class/pwm"

int pwm_open(int chip, int pin)
{
	int fd, len, status;
	char pwm_dev[MAX_BUF_LEN] = {0};
	char pwm_buf[MAX_BUF_LEN] = {0};

	_D("chip : %d, pin : %d", chip, pin);

	snprintf(pwm_dev, MAX_BUF_LEN, SYSFS_PWM_PATH "/pwmchip%d/export", chip);
	fd = open(pwm_dev, O_WRONLY);
	CHECK_ERROR(fd < 0);

	len = snprintf(pwm_buf, MAX_BUF_LEN, "%d", pin);
	status = write(fd, pwm_buf, len);
	close(fd);
	CHECK_ERROR(status != len);

	return 0;
}

int pwm_close(int chip, int pin)
{
	int fd, len, status;
	char pwm_dev[MAX_BUF_LEN] = {0};
	char pwm_buf[MAX_BUF_LEN] = {0};

	_D("chip : %d, pin : %d", chip, pin);

	snprintf(pwm_dev, MAX_BUF_LEN, SYSFS_PWM_PATH "/pwmchip%d/unexport", chip);
	fd = open(pwm_dev, O_WRONLY);
	CHECK_ERROR(fd < 0);

	len = snprintf(pwm_buf, MAX_BUF_LEN, "%d", pin);
	status = write(fd, pwm_buf, len);
	close(fd);
	CHECK_ERROR(status != len);

	return 0;
}
