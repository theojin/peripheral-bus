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

#ifndef __PERIPHERAL_BUS_GPIO_H__
#define __PERIPHERAL_BUS_GPIO_H__

int peripheral_bus_gpio_open(peripheral_gpio_h gpio);
int peripheral_bus_gpio_set_direction(peripheral_gpio_h gpio);
int peripheral_bus_gpio_get_direction(peripheral_gpio_h gpio);
int peripheral_bus_gpio_set_edge(peripheral_gpio_h gpio);
int peripheral_bus_gpio_get_edge(peripheral_gpio_h gpio);
int peripheral_bus_gpio_write(peripheral_gpio_h gpio, int value);
int peripheral_bus_gpio_read(peripheral_gpio_h gpio, int *read_value);
int peripheral_bus_gpio_close(peripheral_gpio_h gpio);

#endif /* __PERIPHERAL_BUS_GPIO_H__ */
