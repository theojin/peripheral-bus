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

#include "peripheral_interface_spi.h"
#include "peripheral_interface_common.h"

#define SYSFS_SPI_DIR "/dev/spidev"

int spi_open(int bus, int cs, int *fd)
{
	int new_fd = 0;
	char spi_dev[MAX_BUF_LEN] = {0,};

	_D("bus : %d, cs : %d", bus, cs);

	snprintf(spi_dev, MAX_BUF_LEN, SYSFS_SPI_DIR"%d.%d", bus, cs);

	new_fd = open(spi_dev, O_RDWR);
	CHECK_ERROR(new_fd < 0);

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
	CHECK_ERROR(status != 0);

	return 0;
}
