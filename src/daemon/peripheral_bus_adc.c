/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <gio/gio.h>

#include <peripheral_io.h>

#include "adc.h"
#include "peripheral_bus.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"

#define SYSFS_ADC_PATH_MAX		128

char *peripheral_bus_adc_get_path(unsigned int device)
{
	char *path;
	int ret;

	path = calloc(1, SYSFS_ADC_PATH_MAX);
	if (path == NULL) {
		_E("failed to allocate path buffer");
		return NULL;
	}

	ret = adc_get_path(device, path, SYSFS_ADC_PATH_MAX);
	if (ret < 0) {
		_E("failed to get adc path");
		free(path);
		return NULL;
	}

	return path;
}

int peripheral_bus_adc_read(unsigned int device, unsigned int channel, char *path, int *data)
{
	if (path == NULL) return PERIPHERAL_ERROR_INVALID_PARAMETER;

	return adc_read(device, channel, path, data);
}
