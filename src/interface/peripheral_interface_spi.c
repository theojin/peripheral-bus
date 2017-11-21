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
#include <linux/spi/spidev.h>

#include "peripheral_interface_spi.h"
#include "peripheral_common.h"

#define SYSFS_SPI_DIR "/dev/spidev"
#define SPI_BUFFER_MAX 64
#define MAX_ERR_LEN 255

int spi_open(int bus, int cs, int *fd)
{
	int new_fd = 0;
	char spi_dev[SPI_BUFFER_MAX] = {0,};

	_D("bus : %d, cs : %d", bus, cs);

	snprintf(spi_dev, sizeof(spi_dev)-1, SYSFS_SPI_DIR"%d.%d", bus, cs);

	new_fd = open(spi_dev, O_RDWR);
	if (new_fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", spi_dev, errmsg);
		return -ENXIO;
	}
	_D("fd : %d", new_fd);
	*fd = new_fd;

	return 0;
}

int spi_close(int fd)
{
	int status;

	_D("fd : %d", fd);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = close(fd);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to close fd : %d", fd);
		return -EIO;
	}

	return 0;
}
