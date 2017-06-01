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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "i2c.h"
#include "peripheral_common.h"

#define MAX_ERR_LEN 255

int i2c_open(int bus, int *fd)
{
	int new_fd;
	char i2c_dev[I2C_BUFFER_MAX] = {0,};

	snprintf(i2c_dev, sizeof(i2c_dev)-1, SYSFS_I2C_DIR"-%d", bus);
	new_fd = open(i2c_dev, O_RDWR);

	if (new_fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s : %s", i2c_dev, errmsg);
		return -ENODEV;
	}
	*fd = new_fd;

	return 0;
}

int i2c_close(int fd)
{
	int status;

	if (fd < 0) return -EINVAL;

	status = close(fd);

	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to close fd : %d", fd);
		return -EIO;
	}

	return 0;
}

int i2c_set_address(int fd, int address)
{
	int status;

	_D("slave address : %x", address);
	status = ioctl(fd, I2C_SLAVE, address);

	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to set slave address(%x) : %s", address, errmsg);
		return status;
	}

	return 0;
}

int i2c_read(int fd, unsigned char *data, int length)
{
	int status;

	_D("fd : %d, length : %d", fd, length);
	status = read(fd, data, length);

	if (status != length) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("i2c_read failed : %s", errmsg);
		return -EIO;
	}

	return 0;
}

int i2c_write(int fd, const unsigned char *data, int length)
{
	int status;

	_D("fd : %d, length : %d", fd, length);
	status = write(fd, data, length);

	if (status != length) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("i2c write failed : %s\n", errmsg);
		return -EIO;
	}

	return 0;
}

int i2c_smbus_ioctl(int fd, struct i2c_smbus_ioctl_data *data)
{
	int status;

	_D("fd : %d", fd);
	status = ioctl(fd, I2C_SMBUS, data);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("i2c write failed : %s\n", errmsg);
		return -EIO;
	}

	return 0;
}
