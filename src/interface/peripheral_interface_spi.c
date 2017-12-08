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

static int __peripheral_interface_spi_fd_open(int bus, int cs, int *fd_out)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi bus");
	RETVM_IF(cs < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi cs");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for spi");

	int fd = 0;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/dev/spidev%d.%d", bus, cs);

	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_spi_fd_list_create(int bus, int cs, GUnixFDList **list_out)
{
	RETVM_IF(bus < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi bus");
	RETVM_IF(cs < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid spi chip select");

	int ret;

	GUnixFDList *list = NULL;
	int fd = -1;


	ret = __peripheral_interface_spi_fd_open(bus, cs, &fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open spi fd");
	}

	list = g_unix_fd_list_new();
	if (list == NULL) {
		_E("Failed to create spi fd list");
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

void peripheral_interface_spi_fd_list_destroy(GUnixFDList *list)
{
	if (list != NULL)
		g_object_unref(list);
}