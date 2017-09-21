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

#ifndef __PWM_H__
#define __PWM_H__

/**
 * @brief Enumeration for Polarity
 */
typedef enum {
	PWM_POLARITY_NORMAL = 0,
	PWM_POLARITY_INVERSED,
} pwm_polarity_e;

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

/**
* @brief pwm_set_period() sets the pwm period.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[in] period pwm period
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_set_period(int chip, int pin, int period);

/**
* @brief pwm_get_period() gets the pwm period.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[out] period pwm period
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_get_period(int chip, int pin, int *period);

/**
* @brief pwm_set_duty_cycle() sets the pwm duty cycle.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[in] duty_cycle pwm duty cycle
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_set_duty_cycle(int chip, int pin, int duty_cycle);

/**
* @brief pwm_get_duty_cycle() gets the pwm duty cycle.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[out] duty_cycle pwm duty cycle
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_get_duty_cycle(int chip, int pin, int *duty_cycle);

/**
* @brief pwm_set_polarity() sets the pwm polarity.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[in] polarity pwm polarity
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_set_polarity(int chip, int pin, pwm_polarity_e polarity);
/**
* @brief pwm_get_polarity() gets the pwm polarity.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[out] polarity pwm polarity
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_get_polarity(int chip, int pin, pwm_polarity_e *polarity);

/**
* @brief pwm_set_enable() sets the pwm state.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[in] enable pwm enable/disabled state value
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_set_enable(int chip, int pin, bool enable);

/**
* @brief pwm_get_enable() checks if pwm state is enabled.
*
* @param[in] chip pwm chip number
* @param[in] pin pwm pin number
* @param[out] enable pwm enable/disabled state value
* @return On success, 0 is returned. On failure, a negative value is returned.
*/
int pwm_get_enable(int chip, int pin, bool *enable);

#endif /* __PWM_H__ */
