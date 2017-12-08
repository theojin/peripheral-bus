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

#define I2C_SLAVE	0x0703	/* Use this slave address */

static int __peripheral_interface_i2c_fd_open(int bus, int address, int *fd_out)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c bus");
	RETVM_IF(address < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c address");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for i2c");

	int ret;
	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/dev/i2c-%d", bus);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	ret = ioctl(fd, I2C_SLAVE, address);
	IF_ERROR_RETURN(ret != 0, close(fd));

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_i2c_fd_list_create(int bus, int address, GUnixFDList **list_out)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c bus");
	RETVM_IF(address < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid i2c address");

	int ret;

	GUnixFDList *list = NULL;
	int fd = -1;


	ret = __peripheral_interface_i2c_fd_open(bus, address, &fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open i2c fd");
	}

	list = g_unix_fd_list_new();
	if (list == NULL) {
		_E("Failed to create i2c fd list");
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

void peripheral_interface_i2c_fd_list_destroy(GUnixFDList *list)
{
	if (list != NULL)
		g_object_unref(list);
}