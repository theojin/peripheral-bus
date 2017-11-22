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

#include <sys/ioctl.h>

#include "peripheral_interface_i2c.h"
#include "peripheral_interface_common.h"

#define SYSFS_I2C_DIR "/dev/i2c"
#define I2C_SLAVE	0x0703	/* Use this slave address */

int i2c_open(int bus, int *fd)
{
	int new_fd;
	char i2c_dev[MAX_BUF_LEN] = {0,};

	_D("bus : %d", bus);

	snprintf(i2c_dev, MAX_BUF_LEN, SYSFS_I2C_DIR"-%d", bus);
	new_fd = open(i2c_dev, O_RDWR);
	IF_ERROR_RETURN(new_fd < 0);

	_D("fd : %d", new_fd);
	*fd = new_fd;

	return 0;
}

int i2c_close(int fd)
{
	int status;

	_D("fd : %d", fd);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = close(fd);
	IF_ERROR_RETURN(status != 0);

	return 0;
}

int i2c_set_address(int fd, int address)
{
	int status;

	_D("fd : %d, slave address : 0x%x", fd, address);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = ioctl(fd, I2C_SLAVE, address);
	IF_ERROR_RETURN(status != 0, close(fd));
	close(fd);

	return 0;
}