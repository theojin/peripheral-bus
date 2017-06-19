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

#ifndef __PERIPHERAL_BUS_SPI_H__
#define __PERIPHERAL_BUS_SPI_H__

int peripheral_bus_spi_open(int bus, int cs, pb_data_h *handle, gpointer user_data);
int peripheral_bus_spi_close(pb_data_h handle);
int peripheral_bus_spi_set_mode(pb_data_h handle, unsigned char mode);
int peripheral_bus_spi_get_mode(pb_data_h handle, unsigned char *mode);
int peripheral_bus_spi_set_lsb_first(pb_data_h handle, gboolean lsb);
int peripheral_bus_spi_get_lsb_first(pb_data_h handle, gboolean *lsb);
int peripheral_bus_spi_set_bits(pb_data_h handle, unsigned char bits);
int peripheral_bus_spi_get_bits(pb_data_h handle, unsigned char *bits);
int peripheral_bus_spi_set_frequency(pb_data_h handle, unsigned int freq);
int peripheral_bus_spi_get_frequency(pb_data_h handle, unsigned int *freq);
int peripheral_bus_spi_read(pb_data_h handle, GVariant **data_array, int length);
int peripheral_bus_spi_write(pb_data_h handle, GVariant *data_array, int length);
int peripheral_bus_spi_read_write(pb_data_h handle, GVariant *tx_data_array, GVariant **rx_data_array, int length);

#endif /* __PERIPHERAL_BUS_SPI_H__ */
