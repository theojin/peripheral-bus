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

#ifndef __PERIPHERAL_INTERFACE_PWM_H__
#define __PERIPHERAL_INTERFACE_PWM_H__

/**
* @brief pwm_open() init pwm pin.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_open(int chip, int pin);

/**
* @brief pwm_close() deinit pwm pin.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_close(int chip, int pin);

#endif /* __PERIPHERAL_INTERFACE_PWM_H__ */
