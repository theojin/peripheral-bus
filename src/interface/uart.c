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

#define PATH_BUF_MAX 64

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif
#define MAX_ERR_LEN 128

char *sysfs_uart_path[] = {
	"/dev/ttyS",
	"/dev/ttyAMA",
	"/dev/ttySAC",
};

int uart_open(int port, int *file_hndl)
{
	int i, fd;
	char uart_dev[PATH_BUF_MAX] = {0};

	_D("port : %d", port);

	for (i = 0; i < ARRAY_SIZE(sysfs_uart_path); i++) {
		snprintf(uart_dev, PATH_BUF_MAX, "%s%d", sysfs_uart_path[i], port);
		if (access(uart_dev, F_OK) == 0)
			break;
	}
	_D("uart_dev : %s", uart_dev);
	if ((fd = open(uart_dev, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("can't open %s, errmsg : %s", uart_dev, errmsg);
		return -ENXIO;
	}
	*file_hndl = fd;
	return 0;
}

int uart_close(int file_hndl)
{
	int status;

	_D("file_hndl : %d", file_hndl);

	if (file_hndl < 0) {
		_E("Invalid NULL parameter");
		return -EINVAL;
	}

	status = uart_flush(file_hndl);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to close fd : %d, errmsg : %s", file_hndl, errmsg);
		return -EIO;
	}

	status = close(file_hndl);
	if (status < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Failed to close fd : %d, errmsg : %s", file_hndl, errmsg);
		return -EIO;
	}

	return 0;
}

int uart_flush(int file_hndl)
{
	int ret;

	if (!file_hndl) {
		_E("Invalid NULL parameter");
		return -EINVAL;
	}

	ret = tcflush(file_hndl, TCIOFLUSH);
	if (ret < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("tcflush failed, errmsg : %s", errmsg);
		return -1;
	}

	return 0;
}
