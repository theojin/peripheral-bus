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

static int __peripheral_interface_uart_fd_open(int port, int *fd_out)
{
	RETVM_IF(port < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid uart port");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for uart");

	static char *sysfs_uart_path[] = {
#if defined(SDTA7D)
		"/dev/ttymxc",
#endif
		"/dev/ttyS",
		"/dev/ttyAMA",
		"/dev/ttySAC",
	};

	int index;
	int fd;
	char path[MAX_BUF_LEN] = {0, };

	for (index = 0; index < ARRAY_SIZE(sysfs_uart_path); index++) {
		snprintf(path, MAX_BUF_LEN, "%s%d", sysfs_uart_path[index], port);
		if (access(path, F_OK) == 0)
			break;
	}

	fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_uart_fd_list_create(int port, GUnixFDList **list_out)
{
	RETVM_IF(port < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid uart port");

	int ret;

	GUnixFDList *list = NULL;
	int fd = -1;


	ret = __peripheral_interface_uart_fd_open(port, &fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open uart fd");
	}

	list = g_unix_fd_list_new();
	if (list == NULL) {
		_E("Failed to create uart fd list");
		ret = PERIPHERAL_ERROR_OUT_OF_MEMORY;
		goto out;
	}

	/* Do not change the order of the fd list */
	g_unix_fd_list_append(list, fd, NULL);

	*list_out = list;

out:
	close(fd);

	return ret;
}

void peripheral_interface_uart_fd_list_destroy(GUnixFDList *list)
{
	if (list != NULL)
		g_object_unref(list);
}
