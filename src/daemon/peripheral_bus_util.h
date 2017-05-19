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

#ifndef __PERIPHERAL_UTIL_H__
#define __PERIPHERAL_UTIL_H__

int peripheral_bus_get_client_info(GDBusMethodInvocation *invocation, peripheral_bus_s *pb_data, pb_client_info_s *client_info);
GVariant *peripheral_bus_build_variant_ay(uint8_t *data, int length);

#endif /* __PERIPHERAL_UTIL_H__ */
