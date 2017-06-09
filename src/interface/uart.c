/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/ioctl.h>

#include "uart.h"
#include "peripheral_common.h"

#define SYSFS_UART_PATH		"/dev/ttySAC"

#define PATH_BUF_MAX		64
#define UART_BUF_MAX		16

#define UART_BAUDRATE_SIZE	19

static const int peripheral_uart_br[UART_BAUDRATE_SIZE] = {
	B0,			B50,		B75,		B110,		B134,
	B150,		B200,		B300,		B600,		B1200,
	B1800,		B2400,		B4800,		B9600,		B19200,
	B38400,		B57600,		B115200,	B230400
};

static const int byteinfo[4] = {CS5, CS6, CS7, CS8};

int uart_open(int port, int *file_hndl)
{
	int fd;
	char fName[PATH_BUF_MAX] = {0};

	_D("port : %d", port);

	snprintf(fName, PATH_BUF_MAX, SYSFS_UART_PATH "%d", port);
	if ((fd = open(fName, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		_E("Error[%d]: can't open %s, %s--[%d]\n", errno, fName, __FUNCTION__, __LINE__);
		return -ENXIO;
	}
	*file_hndl = fd;
	return 0;
}

int uart_close(int file_hndl)
{
	_D("file_hndl : %d", file_hndl);

	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}
	close(file_hndl);
	return 0;
}

int uart_flush(int file_hndl)
{
	int ret;

	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = tcflush(file_hndl, TCIOFLUSH);
	if (ret < 0) {
		_E("FAILED[%d]: tcflush, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int uart_set_baudrate(int file_hndl, uart_baudrate_e baud)
{
	int ret;
	struct termios tio;

	_D("file_hndl : %d, baud : %d", file_hndl, baud);

	memset(&tio, 0, sizeof(tio));
	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	if (baud > UART_BAUDRATE_230400) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = tcgetattr(file_hndl, &tio);
	if (ret) {
		_E("Error[%d]: tcgetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}
	tio.c_cflag = peripheral_uart_br[baud];
	tio.c_iflag = IGNPAR;
	tio.c_oflag = 0;
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 0;

	uart_flush(file_hndl);
	ret = tcsetattr(file_hndl, TCSANOW, &tio);
	if (ret) {
		_E("Error[%d]: tcsetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int uart_set_mode(int file_hndl, uart_bytesize_e bytesize, uart_parity_e parity, uart_stopbits_e stopbits)
{
	int ret;
	struct termios tio;

	_D("file_hndl : %d, bytesize : %d, parity : %d, stopbits : %d", file_hndl, bytesize, parity, stopbits);

	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = tcgetattr(file_hndl, &tio);
	if (ret) {
		_E("Error[%d]: tcgetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	/* set byte size */
	if (bytesize < UART_BYTESIZE_5BIT || bytesize > UART_BYTESIZE_8BIT) {
		_E("Error[%d]: Invalid parameter bytesize, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}
	tio.c_cflag &= ~CSIZE;
	tio.c_cflag |= byteinfo[bytesize];
	tio.c_cflag |= (CLOCAL | CREAD);

	/* set parity info */
	switch (parity) {
	case UART_PARITY_EVEN:
		tio.c_cflag |= PARENB;
		tio.c_cflag &= ~PARODD;
		break;
	case UART_PARITY_ODD:
		tio.c_cflag |= PARENB;
		tio.c_cflag |= PARODD;
		break;
	case UART_PARITY_NONE:
	default:
		tio.c_cflag &= ~PARENB;
		tio.c_cflag &= ~PARODD;
		break;
	}

	/* set stop bit */
	switch (stopbits) {
	case UART_STOPBITS_1BIT:
		tio.c_cflag &= ~CSTOPB;
		break;
	case UART_STOPBITS_2BIT:
		tio.c_cflag |= CSTOPB;
		break;
	default:
		_E("Error[%d]: Invalid parameter stopbits, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	uart_flush(file_hndl);
	ret = tcsetattr(file_hndl, TCSANOW, &tio);
	if (ret) {
		_E("Error[%d]: tcsetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int uart_set_flowcontrol(int file_hndl, bool xonxoff, bool rtscts)
{
	int ret;
	struct termios tio;

	_D("file_hndl : %d, xonxoff : %d, rtscts : %d", file_hndl, xonxoff, rtscts);

	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = tcgetattr(file_hndl, &tio);
	if (ret) {
		_E("Error[%d]: tcgetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	/* rtscts => 1: rts/cts on, 0: off */
	if (rtscts)
		tio.c_cflag |= CRTSCTS;
	else
		tio.c_cflag &= ~CRTSCTS;

	/* xonxoff => 1: xon/xoff on, 0: off */
	if (xonxoff)
		tio.c_iflag |= (IXON | IXOFF | IXANY);
	else
		tio.c_iflag &= ~(IXON | IXOFF | IXANY);

	ret = tcsetattr(file_hndl, TCSANOW, &tio);
	if (ret) {
		_E("Error[%d]: tcsetattr, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int uart_read(int file_hndl, uint8_t *buf, unsigned int length)
{
	int ret;
	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = read(file_hndl, (void *)buf, length);
	if ((errno != EAGAIN && errno != EINTR) && ret < 0) {
		_E("Error[%d]: read, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EIO;
	}

	return ret;
}

int uart_write(int file_hndl, uint8_t *buf, unsigned int length)
{
	int ret;
	if (!file_hndl) {
		_E("Error[%d]: Invalid parameter, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EINVAL;
	}

	ret = write(file_hndl, buf, length);
	if (ret < 0) {
		_E("Error[%d]: write, %s--[%d]\n", errno, __FUNCTION__, __LINE__);
		return -EIO;
	}

	return ret;
}
