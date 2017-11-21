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

#ifndef __PERIPHERAL_INTERFACE_UART_H__
#define __PERIPHERAL_INTERFACE_UART_H__

/**
* @brief uart_open() initializes uart port.
*
* @param[in] port uart port
* @param[in] file_hndl handle of uart port
* @return On success, handle of uart_context is returned. On failure, NULL is returned.
*/
int uart_open(int port, int *file_hndl);

/**
* @brief uart_close() closes uart port.
*
* @param[in] file_hndl handle of uart_context
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_close(int file_hndl);

#endif /* __PERIPHERAL_INTERFACE_UART_H__ */

