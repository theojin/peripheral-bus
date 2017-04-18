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
#include <linux/i2c.h>

#include "i2c.h"
#include "peripheral_common.h"

#define MAX_ERR_LEN 255

int i2c_open(int bus, int *file_hndl)
{
	int fd;
	char i2c_dev[I2C_BUFFER_MAX] = {0,};

	snprintf(i2c_dev, sizeof(i2c_dev)-1, SYSFS_I2C_DIR"-%d", bus);

	fd = open(i2c_dev, O_RDWR);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /dev/i2c-%d : %s\n", bus, errmsg);
		return -ENODEV;
	}

	*file_hndl = fd;
	//*file_hndl = bus;
	//close(fd);

	return 0;
}

int i2c_close(int file_hndl)
{
	if (file_hndl == (int)NULL)
		return -EINVAL;

	close(file_hndl);

	return 0;
}

int i2c_set_frequency(int file_hndl, i2c_mode_e speed)
{
	int status;
	int fd;
	char i2c_dev[I2C_BUFFER_MAX] = {0,};
	int frequency = 0;

	snprintf(i2c_dev, sizeof(i2c_dev)-1, SYSFS_I2C_DIR"-%d", file_hndl);
	fd = open(i2c_dev, O_RDWR);

	if (fd < 0)
		return -ENODEV;

	if (speed == I2C_STD) {
		frequency = 10000;
	} else if (speed == I2C_FAST) {
		frequency = 400000;
	} else if (speed == I2C_HIGH) {
		frequency = 3400000;
	} else {
		_E("Error: speed is not supported [%d]\n", speed);
		close(fd);
		return -EINVAL;
	}

	status = ioctl(fd, I2C_FREQUENCY, (unsigned long)((unsigned int*)&frequency));

	if (status < 0) {
		_E("Error  I2C_FREQUENCY, speed[%d]:\n",  speed);
		close(fd);
		return -EIO;
	}
	close(fd);
	return 0;
}

int i2c_set_address(int file_hndl, int address)
{
	int status;

	_D("I2C SLAVE address = [%x]\n", address);

	status = ioctl(file_hndl, I2C_SLAVE, address);

	if (status < 0) {
		_E("Error I2C_SLAVE, address[%x]:\n", address);
		return -EIO;
	}

	return 0;
}

int i2c_read(int file_hndl, unsigned char *data, int length, int addr)
{
	int status;
	//int fd;
	//char i2c_dev[I2C_BUFFER_MAX] = {0,};

	//snprintf(i2c_dev, sizeof(i2c_dev)-1, SYSFS_I2C_DIR"-%d", file_hndl);

	//fd = open(i2c_dev, O_RDWR);

	//if (fd < 0) {
	//	_E("Can't Open /dev/i2c-%d : %s\n", file_hndl, strerror(errno));
	//	return PERIPHERAL_ERROR_INVALID_PARAMETER;
	//}
	//status = ioctl(fd, I2C_SLAVE, addr);
	//if (status < 0) {
	//	_E("Error I2C_SLAVE, address[%x]:\n", addr);
	//	return PERIPHERAL_ERROR_UNKNOWN;
	//}
	_D("[Read] file_hndle = %d\n", file_hndl);
	status = read(file_hndl, data, length);

	if (status != length) {
		_E("i2c transaction read failed\n");
		return -EIO;
	} else
		_D("[SUCCESS] data[%02x][%02x]\n", data[0], data[1]);

	//close(fd);

	return 0;
}

int i2c_write(int file_hndl, const unsigned char *data, int length, int addr)
{
	int status;
	//int fd;
	//char i2c_dev[I2C_BUFFER_MAX] = {0,};

	//snprintf(i2c_dev, sizeof(i2c_dev)-1, SYSFS_I2C_DIR"-%d", file_hndl);

	//fd = open(i2c_dev, O_RDWR);

	//if (fd < 0) {
	//	_E("Can't Open /dev/i2c-%d : %s\n", file_hndl, strerror(errno));
	//	return PERIPHERAL_ERROR_INVALID_PARAMETER;
	//}

//	status = ioctl(fd, I2C_SLAVE, addr);
	//if (status < 0) {
	//	_E("Error I2C_SLAVE, address[%x]:\n", addr);
	//	return PERIPHERAL_ERROR_UNKNOWN;
	//}

	status = write(file_hndl, data, length);

	if (status != length) {
		_E("i2c transaction wrtie failed \n");
	//	close(fd);
		return -EIO;
	}

	//close(fd);

	return 0;
}
