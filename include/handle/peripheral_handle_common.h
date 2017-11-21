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

#ifndef __PERIPHERAL_HANDLE_COMMON_H__
#define __PERIPHERAL_HANDLE_COMMON_H__

#include <gio/gio.h>

#include "peripheral_handle.h"
#include "peripheral_log.h"

pb_data_h peripheral_bus_data_new(GList **plist);
int peripheral_bus_data_free(pb_data_h handle);

#endif /* __PERIPHERAL_HANDLE_COMMON_H__ */
