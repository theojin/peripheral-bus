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

#include <stdlib.h>

#include "peripheral_handle_common.h"

pb_data_h peripheral_bus_data_new(GList **plist)
{
	GList *list = *plist;
	pb_data_h handle;

	handle = (pb_data_h)calloc(1, sizeof(peripheral_bus_data_s));
	if (handle == NULL) {
		_E("failed to allocate peripheral_bus_data_s");
		return NULL;
	}

	*plist = g_list_append(list, handle);

	return handle;
}

int peripheral_bus_data_free(pb_data_h handle)
{
	GList *list = *handle->list;
	GList *link;

	RETVM_IF(handle == NULL, -1, "handle is null");

	link = g_list_find(list, handle);
	if (!link) {
		_E("handle does not exist in list");
		return -1;
	}

	*handle->list = g_list_remove_link(list, link);

	free(handle);
	g_list_free(link);

	return 0;
}