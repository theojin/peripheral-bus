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

#ifndef __SPI_H__
#define __SPI_H__

int spi_open(int bus, int cs, int *fd);
int spi_close(int fd);
int spi_set_mode(int fd, unsigned char mode);
int spi_set_bit_order(int fd, unsigned char lsb);
int spi_set_bits_per_word(int fd, unsigned char bits);
int spi_set_frequency(int fd, unsigned int freq);
int spi_get_buffer_size(int *bufsiz);
int spi_read(int fd, unsigned char *rxbuf, int length);
int spi_write(int fd, unsigned char *txbuf, int length);
int spi_transfer(int fd, unsigned char *txbuf, unsigned char *rxbuf, int length);

#endif /* __SPI_H__ */
