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

#include <libudev.h>
#include <poll.h>

#include "peripheral_interface_gpio.h"
#include "peripheral_interface_common.h"

static int __gpio_wait_for_udev(int pin)
{
	#define GPIO_NAME_LEN 8
	struct udev *udev;
	struct udev_monitor *monitor;
	struct udev_device *dev;
	struct pollfd pfd;
	char gpio_name[GPIO_NAME_LEN];
	int ret = -EIO;

	udev = udev_new();
	if (!udev) {
		_E("Cannot create udev");
		return ret;
	}

	monitor = udev_monitor_new_from_netlink(udev, "udev");
	if (!monitor) {
		_E("Cannot create udev monitor");
		udev_unref(udev);
		return ret;
	}

	ret = udev_monitor_filter_add_match_subsystem_devtype(monitor, "gpio", NULL);
	if (ret < 0) {
		_E("Failed to add monitor filter");
		goto out;
	}

	ret = udev_monitor_enable_receiving(monitor);
	if (ret < 0) {
		_E("Failed to enable udev receiving");
		goto out;
	}

	pfd.fd = udev_monitor_get_fd(monitor);
	pfd.events = POLLIN;

	snprintf(gpio_name, GPIO_NAME_LEN, "gpio%d", pin);

	for (int cnt = 0; cnt < 10; cnt++) {
		if (poll(&pfd, 1, 100) < 0) {
			_E("Failed to watch udev monitor");
			goto out;
		}

		dev = udev_monitor_receive_device(monitor);
		if (dev) {
			if (strcmp(udev_device_get_sysname(dev), gpio_name) == 0) {
				_D("udev for %s is initialized", gpio_name);
				ret = 0;
				goto out;
			}
		}
	}
	_E("Time out");

out:
	udev_monitor_unref(monitor);
	udev_unref(udev);

	return ret;
}

int peripheral_interface_gpio_export(int pin)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");

	int ret;
	int fd;
	int length;
	char buf[MAX_BUF_LEN] = {0, };

	fd = open("/sys/class/gpio/export", O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	ret = __gpio_wait_for_udev(pin);
	if (ret < 0) {
		_E("device nodes are not writable");
		return PERIPHERAL_ERROR_IO_ERROR;
	}

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_unexport(int pin)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");

	int ret;
	int fd;
	int length;
	char buf[MAX_BUF_LEN] = {0, };

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	IF_ERROR_RETURN(fd < 0);

	length = snprintf(buf, MAX_BUF_LEN, "%d", pin);
	ret = write(fd, buf, length);
	IF_ERROR_RETURN(ret != length, close(fd));

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_direction_open(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio direction");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_direction_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for gpio direction");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_edge_open(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio edge");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/edge", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_edge_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for gpio edge");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_value_open(int pin, int *fd_out)
{
	RETVM_IF(pin < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid gpio pin");
	RETVM_IF(fd_out == NULL, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd_out for gpio value");

	int fd;
	char path[MAX_BUF_LEN] = {0, };

	snprintf(path, MAX_BUF_LEN, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDWR);
	IF_ERROR_RETURN(fd < 0);

	*fd_out = fd;

	return PERIPHERAL_ERROR_NONE;
}

int peripheral_interface_gpio_fd_value_close(int fd)
{
	RETVM_IF(fd < 0, PERIPHERAL_ERROR_INVALID_PARAMETER, "Invalid fd for gpio value");

	int ret;

	ret = close(fd);
	IF_ERROR_RETURN(ret != 0);

	return PERIPHERAL_ERROR_NONE;
}

