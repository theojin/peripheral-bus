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

#ifndef __PERIPHERAL_BUS_UART_H__
#define __PERIPHERAL_BUS_UART_H__

int peripheral_bus_uart_open(int port, pb_data_h *handle, gpointer user_data);
int peripheral_bus_uart_close(pb_data_h handle);
int peripheral_bus_uart_set_baud_rate(pb_data_h handle, int baud_rate);
int peripheral_bus_uart_set_byte_size(pb_data_h handle, int byte_size);
int peripheral_bus_uart_set_parity(pb_data_h handle, int parity);
int peripheral_bus_uart_set_stop_bits(pb_data_h handle, int stop_bits);
int peripheral_bus_uart_set_flow_control(pb_data_h handle, int xonxoff, int rtscts);
int peripheral_bus_uart_read(pb_data_h handle, GVariant **data_array, int length);
int peripheral_bus_uart_write(pb_data_h handle, GVariant *data_array, int length);

#endif /* __PERIPHERAL_BUS_UART_H__ */
