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

int peripheral_bus_gpio_open(gint pin, pb_gpio_data_h *gpio, gpointer user_data);
int peripheral_bus_gpio_set_direction(pb_gpio_data_h gpio, gint direction);
int peripheral_bus_gpio_get_direction(pb_gpio_data_h gpio, gint *direction);
int peripheral_bus_gpio_set_edge(pb_gpio_data_h gpio, gint edge);
int peripheral_bus_gpio_get_edge(pb_gpio_data_h gpio, gint *edge);
int peripheral_bus_gpio_write(pb_gpio_data_h gpio, gint value);
int peripheral_bus_gpio_read(pb_gpio_data_h gpio, gint *value);
int peripheral_bus_gpio_register_irq(pb_gpio_data_h gpio);
int peripheral_bus_gpio_unregister_irq(pb_gpio_data_h gpio);
int peripheral_bus_gpio_close(pb_gpio_data_h gpio);

#endif /* __PERIPHERAL_BUS_GPIO_H__ */
