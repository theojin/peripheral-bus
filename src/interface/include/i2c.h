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

#ifndef __I2C_H__
#define __I2C_H__

#define SYSFS_I2C_DIR "/dev/i2c"
#define I2C_BUFFER_MAX 64
#define I2C_SLAVE 0x0703

int i2c_open(int bus, int *fd);
int i2c_close(int fd);
int i2c_set_address(int fd, int address);
int i2c_read(int fd, unsigned char *data, int length);
int i2c_write(int fd, const unsigned char *data, int length);

#endif/* __I2C_H__ */
