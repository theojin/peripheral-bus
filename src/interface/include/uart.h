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

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

/**
 * @brief Enumeration for Baud Rate
 */
typedef enum {
	UART_BAUD_RATE_0 = 0,
	UART_BAUD_RATE_50,
	UART_BAUD_RATE_75,
	UART_BAUD_RATE_110,
	UART_BAUD_RATE_134,
	UART_BAUD_RATE_150,
	UART_BAUD_RATE_200,
	UART_BAUD_RATE_300,
	UART_BAUD_RATE_600,
	UART_BAUD_RATE_1200,
	UART_BAUD_RATE_1800,
	UART_BAUD_RATE_2400,
	UART_BAUD_RATE_4800,
	UART_BAUD_RATE_9600,
	UART_BAUD_RATE_19200,
	UART_BAUD_RATE_38400,
	UART_BAUD_RATE_57600,
	UART_BAUD_RATE_115200,
	UART_BAUD_RATE_230400
} uart_baud_rate_e;

/**
 * @brief Enumeration for Byte Size
 */
typedef enum {
	UART_BYTE_SIZE_5BIT = 0,
	UART_BYTE_SIZE_6BIT,
	UART_BYTE_SIZE_7BIT,
	UART_BYTE_SIZE_8BIT
} uart_byte_size_e;

/**
 * @brief Enumeration of Parity Bit
 */
typedef enum {
	UART_PARITY_NONE = 0,
	UART_PARITY_EVEN,
	UART_PARITY_ODD
} uart_parity_e;

/**
 * @brief Enumeration for Stop Bits
 */
typedef enum {
	UART_STOP_BITS_1BIT = 0,
	UART_STOP_BITS_2BIT
} uart_stop_bits_e;

/**
* @brief uart_valid_baudrate() validation check of input baudrate
*
* @param[in] baudrate baudrate for uart
* @return On success, valid input. On failure, NULL is returned.
*/
int uart_valid_baud_rate(unsigned int baud_rate);

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

/**
* @brief uart_flush() flushes uart buffer.
*
* @param[in] file_hndl handle of uart_context
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_flush(int file_hndl);

/**
* @brief uart_set_baudrate() sets uart baud rate.
*
* @param[in] file_hndl handle of uart_context
* @param[in] baud uart baud rate
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_baud_rate(int file_hndl, uart_baud_rate_e baud);

/**
* @brief uart_set_mode() sets byte size, parity bit and stop bits.
*
* @param[in] file_hndl handle of uart_context
* @param[in] byte_size uart byte size
* @param[in] parity uart parity type (even/odd/none)
* @param[in] stop_bits uart stop bits
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_mode(int file_hndl, uart_byte_size_e byte_size, uart_parity_e parity, uart_stop_bits_e stop_bits);

/**
* @brief peripheral_bus_uart_set_byte_size() set byte size.
*
* @param[in] file_hndl handle of uart_context
* @param[in] byte_size uart byte size
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_byte_size(int file_hndl, uart_byte_size_e byte_size);

/**
* @brief peripheral_bus_uart_set_parity() set parity bit.
*
* @param[in] file_hndl handle of uart_context
* @param[in] parity uart parity type (even/odd/none)
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_parity(int file_hndl, uart_parity_e parity);

/**
* @brief peripheral_bus_uart_set_stop_bits() set stop bits.
*
* @param[in] file_hndl handle of uart_context
* @param[in] stop_bits uart stop bits
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_stop_bits(int file_hndl, uart_stop_bits_e stop_bits);

/**
* @brief uart_set_flow_control() set flow control settings.
*
* @param[in] file_hndl handle of uart_context
* @param[in] xonxoff ixon/ixoff
* @param[in] rtscts rts/cts
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_flow_control(int file_hndl, bool xonxoff, bool rtscts);

/**
* @brief uart_read() reads data over uart bus.
*
* @param[in] file_hndl handle of uart_context
* @param[in] buf the pointer of data buffer
* @param[in] length size to read
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_read(int file_hndl, uint8_t *buf, unsigned int length);

/**
* @brief uart_write() writes data over uart bus.
*
* @param[in] file_hndl handle of uart_context
* @param[in] buf the pointer of data buffer
* @param[in] length size to write
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_write(int file_hndl, uint8_t *buf, unsigned int length);

#endif /* __UART_H__ */

