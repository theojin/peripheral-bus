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

#ifndef __ADC_H__
#define __ADC_H__

/**
* @brief adc_get_path() get adc device path.
*
* @param[in] device The adc device number
* @param[in] channel The adc channel number
* @param[out] *path The buffer for the adc device path
* @param[in] length The max length of the path buffer
* @return 0 on success, otherwise a negative error value
*/
int adc_get_path(unsigned int device, char *path, int length);

/**
* @brief adc_read() get adc data.
*
* @param[in] device The adc device number
* @param[in] channel The adc channel number
* @param[in] *path The adc device path
* @param[in] *data The adc value
* @return 0 on success, otherwise a negative error value
*/
int adc_read(unsigned int device, unsigned int channel, char *path, int *data);

#endif /* __ADC_H__ */
