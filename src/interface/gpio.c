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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include "gpio.h"
#include "peripheral_common.h"

#define MAX_ERR_LEN 255

int gpio_open(int gpiopin)
{
	int fd, len, status;
	char gpio_export[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/export :%s\n", errmsg);
		return -ENXIO;
	}

	len = snprintf(gpio_export, GPIO_BUFFER_MAX, "%d", gpiopin);
	status = write(fd, gpio_export, len);

	if (status != len) {
		close(fd);
		_E("Error: gpio open error \n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_set_direction(int gpiopin, gpio_direction_e dir)
{
	int fd, status;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d, dir : %d", gpiopin, dir);

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/direction", gpiopin);
	fd = open(gpio_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d/direction: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	if (dir == GPIO_DIRECTION_IN)
		status = write(fd, "in", strlen("in")+1);
	else if (dir == GPIO_DIRECTION_OUT_HIGH)
		status = write(fd, "high", strlen("high")+1);
	else if (dir == GPIO_DIRECTION_OUT_LOW)
		status = write(fd, "low", strlen("low")+1);
	else {
		close(fd);
		_E("Error: gpio direction is wrong\n");
		return -EIO;
	}

	if (status <= 0) {
		close(fd);
		_E("Error: gpio direction set error\n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_get_direction(int gpiopin, gpio_direction_e *dir)
{
	int fd, len;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };
	char gpio_buf[GPIO_BUFFER_MAX] = {0, };

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/direction", gpiopin);
	fd = open(gpio_dev, O_RDONLY);

	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d/direction: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	len = read(fd, &gpio_buf, GPIO_BUFFER_MAX);
	if (len <= 0) {
		close(fd);
		_E("Error: gpio direction read error\n");
		return -EIO;
	}

	if (0 == strncmp(gpio_buf, "in", strlen("in")))
		*dir = GPIO_DIRECTION_IN;
	else if (0 == strncmp(gpio_buf, "out", strlen("out")))
		*dir = GPIO_DIRECTION_OUT_LOW;
	else {
		close(fd);
		_E("Error: gpio direction is wrong\n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_set_edge_mode(int gpiopin, gpio_edge_e edge)
{
	int fd, status;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d, edge : %d", gpiopin, edge);

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/edge", gpiopin);
	fd = open(gpio_dev, O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d/edge: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	if (edge == GPIO_EDGE_NONE)
		status = write(fd, "none", strlen("none")+1);
	else if (edge == GPIO_EDGE_RISING)
		status = write(fd, "rising", strlen("rising")+1);
	else if (edge == GPIO_EDGE_FALLING)
		status = write(fd, "falling", strlen("falling")+1);
	else if (edge == GPIO_EDGE_BOTH)
		status = write(fd, "both", strlen("both")+1);
	else {
		close(fd);
		_E("Error: gpio edge is wrong\n");
		return -EIO;
	}

	if (status <= 0) {
		close(fd);
		_E("Error: gpio edge set error\n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_get_edge_mode(int gpiopin, gpio_edge_e *edge)
{
	int fd, len;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };
	char gpio_buf[GPIO_BUFFER_MAX] = {0, };

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/edge", gpiopin);
	fd = open(gpio_dev, O_RDONLY);

	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d/edge: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	len = read(fd, &gpio_buf, GPIO_BUFFER_MAX);
	if (len <= 0) {
		close(fd);
		_E("Error: gpio edge read error\n");
		return -EIO;
	}

	if (0 == strncmp(gpio_buf, "none", strlen("none")))
		*edge = GPIO_EDGE_NONE;
	else if (0 == strncmp(gpio_buf, "both", strlen("both")))
		*edge = GPIO_EDGE_BOTH;
	else if (0 == strncmp(gpio_buf, "rising", strlen("rising")))
		*edge = GPIO_EDGE_RISING;
	else if (0 == strncmp(gpio_buf, "falling", strlen("falling")))
		*edge = GPIO_EDGE_FALLING;
	else {
		close(fd);
		_E("Error: gpio edge is wrong\n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_write(int gpiopin, int value)
{
	int fd, status;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/value", gpiopin);
	fd = open(gpio_dev, O_WRONLY);

	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d/value: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	if (value == 1)
		status = write(fd, "1", strlen("1")+1);
	else if (value == 0)
		status = write(fd, "0", strlen("0")+1);
	else {
		close(fd);
		_E("Error: gpio write value error \n");
		return -EIO;
	}

	if (status <= 0) {
		close(fd);
		_E("Error: gpio write error\n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_read(int gpiopin, int *value)
{
	int fd, len;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };
	char gpio_buf[GPIO_BUFFER_MAX] = {0, };

	snprintf(gpio_dev, GPIO_BUFFER_MAX, SYSFS_GPIO_DIR"/gpio%d/value", gpiopin);
	fd = open(gpio_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d pin value: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	len = read(fd, &gpio_buf, 1);
	close(fd);

	if (len <= 0) {
		_E("Error: gpio read error \n");
		return -EIO;
	}

	if (0 == strncmp(gpio_buf, "1", strlen("1")))
		*value = 1;
	else if (0 == strncmp(gpio_buf, "0", strlen("0")))
		*value = 0;
	else {
		_E("Error: gpio value is error \n");
		return -EIO;
	}

	return 0;
}

int gpio_close(int gpiopin)
{
	int fd, len, status;
	char gpio_unexport[GPIO_BUFFER_MAX] = {0, };

	_D("gpiopin : %d", gpiopin);

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/unexport %s\n", errmsg);
		return -ENXIO;
	}

	len = snprintf(gpio_unexport, GPIO_BUFFER_MAX, "%d", gpiopin);
	status = write(fd, gpio_unexport, len);

	if (status != len) {
		close(fd);
		_E("Error: gpio close error \n");
		return -EIO;
	}

	close(fd);

	return 0;
}

int gpio_open_isr(int gpiopin)
{
	int fd;
	char gpio_dev[GPIO_BUFFER_MAX] = {0, };

	snprintf(gpio_dev, sizeof(gpio_dev)-1, SYSFS_GPIO_DIR"/gpio%d/value", gpiopin);

	_D("open isr string [%s]", gpio_dev);

	fd = open(gpio_dev, O_RDONLY);
	if (fd < 0) {
		char errmsg[MAX_ERR_LEN];
		strerror_r(errno, errmsg, MAX_ERR_LEN);
		_E("Can't Open /sys/class/gpio/gpio%d pin value: %s\n", gpiopin, errmsg);
		return -ENXIO;
	}

	return fd;
}

int gpio_close_isr(int fd)
{
	if (fd <= 0) return -EINVAL;

	close(fd);

	return 0;
}

int gpio_read_isr(void *fdset, char *rev_buf, int length)
{
	int poll_state = 0;
	int len;
	struct pollfd poll_events;

	poll_events.fd = ((struct pollfd*)fdset)->fd;
	poll_events.events = POLLPRI;
	poll_events.revents = ((struct pollfd*)fdset)->revents;

	poll_state = poll((struct pollfd*)&poll_events, 1, -1);  // 0 is going to return directly.

	if (poll_state < 0) {
		_E("poll() failed!\n");
		return -EIO;
	}

	if (poll_events.revents & POLLPRI) {
		lseek(poll_events.fd, 0, SEEK_SET);
		len = read(poll_events.fd, rev_buf, length);
		if (len == -1)
			return -EIO;
	}

	return poll_state;
}
