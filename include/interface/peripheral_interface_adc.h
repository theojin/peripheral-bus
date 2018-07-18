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

#ifndef __PERIPHERAL_INTERFACE_ADC_H__
#define __PERIPHERAL_INTERFACE_ADC_H__

#include <gio/gunixfdlist.h>

int peripheral_interface_adc_fd_list_create(int device, int channel, GUnixFDList **list_out);
void peripheral_interface_adc_fd_list_destroy(GUnixFDList *list);

#endif /* __PERIPHERAL_INTERFACE_ADC_H__ */
