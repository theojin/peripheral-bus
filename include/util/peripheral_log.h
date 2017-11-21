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
#ifndef __PERIPHERAL_LOG_H__
#define __PERIPHERAL_LOG_H__

#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "PERIPHERAL-BUS"

#define _E(fmt, arg...) LOGE(fmt, ##arg)
#define _D(fmt, arg...) LOGD(fmt, ##arg)
#define _W(fmt, arg...) LOGW(fmt, ##arg)

#define RET_IF(expr) \
	do { \
		if (expr) { \
			_E("(%s)", #expr); \
			return; \
		} \
	} while (0)
#define RETV_IF(expr, val) \
	do {\
		if (expr) { \
			_E("(%s)", #expr); \
			return (val); \
		} \
	} while (0)
#define RETM_IF(expr, fmt, arg...) \
	do {\
		if (expr) { \
			_E(fmt, ##arg); \
			return; \
		} \
	} while (0)
#define RETVM_IF(expr, val, fmt, arg...) \
	do {\
		if (expr) { \
			_E(fmt, ##arg); \
			return (val); \
		} \
	} while (0)

#endif /* __PERIPHERAL_COMMON_H__ */
