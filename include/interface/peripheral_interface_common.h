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

#ifndef __PERIPHERAL_INTERFACE_COMMON_H__
#define __PERIPHERAL_INTERFACE_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <peripheral_io.h>

#include "peripheral_log.h"

#define MAX_ERR_LEN 255
#define MAX_BUF_LEN 64

#define IF_ERROR_RETURN(expr, func...) \
	do { \
		if (expr) { \
			int temp = errno; \
			func; \
			if (temp == EAGAIN) \
				return PERIPHERAL_ERROR_TRY_AGAIN; \
			char errmsg[MAX_ERR_LEN]; \
			strerror_r(temp, errmsg, sizeof(errmsg)); \
			_E("Failed the %s(%d) function. errmsg: %s", __FUNCTION__, __LINE__, errmsg); \
			return PERIPHERAL_ERROR_IO_ERROR; \
		} \
	} while (0)

#endif /*__PERIPHERAL_INTERFACE_COMMON_H__*/