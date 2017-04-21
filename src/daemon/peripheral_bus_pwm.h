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

#ifndef __PERIPHERAL_BUS_PWM_H__
#define __PERIPHERAL_BUS_PWM_H__

int peripheral_bus_pwm_open(peripheral_pwm_context_h pwm);
int peripheral_bus_pwm_close(peripheral_pwm_context_h pwm);
int peripheral_bus_pwm_set_duty_cycle(peripheral_pwm_context_h pwm, int duty_cycle);
int peripheral_bus_pwm_set_period(peripheral_pwm_context_h pwm, int period);
int peripheral_bus_pwm_set_enable(peripheral_pwm_context_h pwm, int enable);
int peripheral_bus_pwm_get_duty_cycle(peripheral_pwm_context_h pwm, int *duty_cycle);
int peripheral_bus_pwm_get_period(peripheral_pwm_context_h pwm, int*period);

#endif /* __PERIPHERAL_BUS_PWM_H__ */
