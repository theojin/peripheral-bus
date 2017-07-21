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

#include <peripheral_io.h>

#include "peripheral_io_gdbus.h"
#include "peripheral_bus.h"
#include "peripheral_bus_adc.h"
#include "peripheral_common.h"
#include "peripheral_bus_util.h"
#include "peripheral_bus_gdbus_adc.h"

gboolean handle_adc_read(
		PeripheralIoGdbusAdc *adc,
		GDBusMethodInvocation *invocation,
		guint device,
		guint channel,
		gpointer user_data)
{
	peripheral_bus_s *pb_data = (peripheral_bus_s*)user_data;
	peripheral_error_e ret = PERIPHERAL_ERROR_NONE;
	int data = 0;

	if (pb_data->adc_path == NULL) {
		pb_data->adc_path = peripheral_bus_adc_get_path(device);
		if (pb_data->adc_path == NULL) {
			ret = PERIPHERAL_ERROR_UNKNOWN;
			goto out;
		}
	}

	ret = peripheral_bus_adc_read(device, channel, pb_data->adc_path, &data);

out:
	peripheral_io_gdbus_adc_complete_read(adc, invocation, data, ret);

	return true;
}
