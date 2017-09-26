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

#include "spi.h"
#include "peripheral_common.h"

#define SYSFS_SPI_DIR "/dev/spidev"
#define SYSFS_SPI_BUFSIZ "/sys/module/spidev/parameters/bufsiz"
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

int spi_set_mode(int fd, unsigned char mode)
{
	int status;

	_D("fd : %d, mode : %d", fd, mode);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to set mode(%d) : %s", mode, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_set_bit_order(int fd, unsigned char lsb)
{
	int status;

	_D("fd : %d, lsb : %d", fd, lsb);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to set lsb first(%d), fd : %d, errmsg : %s", lsb, fd, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_set_bits_per_word(int fd, unsigned char bits)
{
	int status;

	_D("fd : %d, bits : %d", fd, bits);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to set bits(%d), fd : %d, errmsg : %s", bits, fd, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_set_frequency(int fd, unsigned int freq)
{
	int status;

	_D("fd : %d, freq : %d", fd, freq);
	RETVM_IF(fd < 0, -EINVAL, "Invalid fd");

	status = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &freq);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to set frequency(%d), fd : %d, errmsg : %s", freq, fd, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_get_buffer_size(int *bufsiz)
{
	int fd, result, status;
	char spi_buf[SPI_BUFFER_MAX] = {0,};

	fd = open(SYSFS_SPI_BUFSIZ, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open %s, errmsg : %s", SYSFS_SPI_BUFSIZ, errmsg);
		return -ENXIO;
	}

	status = read(fd, spi_buf, SPI_BUFFER_MAX);
	if (status < 0) {
		_E("Failed to get buffer size, path : %s", SYSFS_SPI_BUFSIZ);
		close(fd);
		return -EIO;
	}
	result = atoi(spi_buf);
	*bufsiz = result;
	close(fd);

	return 0;
}

int spi_read(int fd, unsigned char *rxbuf, int length)
{
	int status;
	struct spi_ioc_transfer xfer;

	RETVM_IF(fd < 0, -EINVAL, "Invalid fd : %d", fd);

	memset(&xfer, 0, sizeof(struct spi_ioc_transfer));
	xfer.rx_buf = (unsigned long)rxbuf;
	xfer.len = length;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to read data, fd : %d, length : %d, errmsg : %s", fd, length, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_write(int fd, unsigned char *txbuf, int length)
{
	int status;
	struct spi_ioc_transfer xfer;

	RETVM_IF(fd < 0, -EINVAL, "Invalid fd : %d", fd);

	memset(&xfer, 0, sizeof(struct spi_ioc_transfer));
	xfer.tx_buf = (unsigned long)txbuf;
	xfer.len = length;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to write data(%d) : %s", length, errmsg);
		return -EIO;
	}

	return 0;
}

int spi_transfer(int fd, unsigned char *txbuf, unsigned char *rxbuf, int length)
{
	int status;
	struct spi_ioc_transfer xfer;

	RETVM_IF(fd < 0, -EINVAL, "Invalid fd : %d", fd);

	if (!txbuf || !rxbuf || length < 0) return -EINVAL;

	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = (unsigned long)txbuf;
	xfer.rx_buf = (unsigned long)rxbuf;
	xfer.len = length;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to exchange data(%d) : %s", length, errmsg);
		return -EIO;
	}

	return 0;
}
