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

#ifndef __GPIO_H__
#define __GPIO_H__

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define GPIO_BUFFER_MAX 64

typedef enum {
	GPIO_DIRECTION_IN = 0,
	GPIO_DIRECTION_OUT_HIGH = 1,
	GPIO_DIRECTION_OUT_LOW = 2,
} gpio_direction_e;

typedef enum {
	GPIO_EDGE_NONE = 0,
	GPIO_EDGE_RISING = 1,
	GPIO_EDGE_FALLING = 2,
	GPIO_EDGE_BOTH = 3,
} gpio_edge_e;

int gpio_open(int gpiopin);
int gpio_close(int gpiopin);
int gpio_set_edge_mode(int gpiopin, gpio_edge_e edge);
int gpio_get_edge_mode(int gpiopin, gpio_edge_e *edge);
int gpio_set_direction(int gpiopin, gpio_direction_e dir);
int gpio_get_direction(int gpiopin, gpio_direction_e *dir);
int gpio_write(int gpiopin, int value);
int gpio_read(int gpiopin, int *value);

int gpio_open_isr(int gpiopin);
int gpio_close_isr(int file_hndl);
int gpio_read_isr(void *fdset, char *rev_buf, int length);
#endif/*__GPIO_H__*/
