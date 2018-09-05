/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include "peripheral_interface_adc.h"
#include "peripheral_interface_common.h"

static int __peripheral_interface_adc_fd_open(int device, int channel, int *fd_out)
{
	RETVM_IF(device < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc device");
	RETVM_IF(channel < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc channel");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for adc");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/bus/iio/devices/iio:device%d/in_voltage%d_raw", device, channel);
	fd = open(path, O_RDONLY);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_adc_fd_list_create(int device, int channel, GUnixFDList **list_out)
{
	RETVM_IF(device < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc device");
	RETVM_IF(channel < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid adc channel");

	int ret;

	GUnixFDList *list = NULL;
	int fd = -1;


	ret = __peripheral_interface_adc_fd_open(device, channel, &fd);
	if (ret != PERIPHERAL_ERROR_NONE) {
		_E("Failed to open adc fd");
	}

	list = g_unix_fd_list_new();
	if (list == NULL) {
		_E("Failed to create adc fd list");
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

void peripheral_interface_adc_fd_list_destroy(GUnixFDList *list)
{
	if (list != NULL)
		g_object_unref(list);
}
