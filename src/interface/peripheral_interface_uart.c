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

#include <termios.h>

#include "peripheral_interface_uart.h"
#include "peripheral_interface_common.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

char *sysfs_uart_path[] = {
	"/dev/ttyS",
	"/dev/ttyAMA",
	"/dev/ttySAC",
};

int uart_open(int port, int *file_hndl)
{
	int i, fd;
	char uart_dev[MAX_BUF_LEN] = {0};

	_D("port : %d", port);

	for (i = 0; i < ARRAY_SIZE(sysfs_uart_path); i++) {
		snprintf(uart_dev, MAX_BUF_LEN, "%s%d", sysfs_uart_path[i], port);
		if (access(uart_dev, F_OK) == 0)
			break;
	}

	_D("uart_dev : %s", uart_dev);
	fd = open(uart_dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
	CHECK_ERROR(fd < 0);

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

	status = tcflush(file_hndl, TCIOFLUSH);
	CHECK_ERROR(status != 0);

	status = close(file_hndl);
	CHECK_ERROR(status != 0);

	return 0;
}