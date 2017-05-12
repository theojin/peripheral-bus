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
	UART_BAUDRATE_0 = 0,
	UART_BAUDRATE_50,
	UART_BAUDRATE_75,
	UART_BAUDRATE_110,
	UART_BAUDRATE_134,
	UART_BAUDRATE_150,
	UART_BAUDRATE_200,
	UART_BAUDRATE_300,
	UART_BAUDRATE_600,
	UART_BAUDRATE_1200,
	UART_BAUDRATE_1800,
	UART_BAUDRATE_2400,
	UART_BAUDRATE_4800,
	UART_BAUDRATE_9600,
	UART_BAUDRATE_19200,
	UART_BAUDRATE_38400,
	UART_BAUDRATE_57600,
	UART_BAUDRATE_115200,
	UART_BAUDRATE_230400
} uart_baudrate_e;

/**
 * @brief Enumeration for Byte Size
 */
typedef enum {
	UART_BYTESIZE_5BIT = 0,
	UART_BYTESIZE_6BIT,
	UART_BYTESIZE_7BIT,
	UART_BYTESIZE_8BIT
} uart_bytesize_e;

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
	UART_STOPBITS_1BIT = 0,
	UART_STOPBITS_2BIT
} uart_stopbits_e;

/**
* @brief uart_valid_baudrate() validation check of input baudrate
*
* @param[in] baudrate baudrate for uart
* @return On success, valid input. On failure, NULL is returned.
*/
int uart_valid_baudrate(unsigned int baudrate);

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
int uart_set_baudrate(int file_hndl, uart_baudrate_e baud);

/**
* @brief uart_set_mode() sets byte size, parity bit and stop bits.
*
* @param[in] file_hndl handle of uart_context
* @param[in] bytesize uart byte size
* @param[in] parity uart parity type (even/odd/none)
* @param[in] stopbits uart stop bits
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_mode(int file_hndl, uart_bytesize_e bytesize, uart_parity_e parity, uart_stopbits_e stopbits);

/**
* @brief uart_set_flowcontrol() set flow control settings.
*
* @param[in] file_hndl handle of uart_context
* @param[in] xonxoff ixon/ixoff
* @param[in] rtscts rts/cts
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int uart_set_flowcontrol(int file_hndl, bool xonxoff, bool rtscts);

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

