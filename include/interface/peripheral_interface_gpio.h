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

#ifndef __PERIPHERAL_INTERFACE_GPIO_H__
#define __PERIPHERAL_INTERFACE_GPIO_H__

int peripheral_interface_gpio_export(int pin);
int peripheral_interface_gpio_unexport(int pin);

int peripheral_interface_gpio_fd_direction_open(int pin, int *fd_out);
int peripheral_interface_gpio_fd_direction_close(int fd);

int peripheral_interface_gpio_fd_edge_open(int pin, int *fd_out);
int peripheral_interface_gpio_fd_edge_close(int fd);

int peripheral_interface_gpio_fd_value_open(int pin, int *fd_out);
int peripheral_interface_gpio_fd_value_close(int fd);

#endif /*__PERIPHERAL_INTERFACE_GPIO_H__*/
