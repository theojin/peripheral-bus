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

#ifndef __PERIPHERAL_BUS_H__
#define __PERIPHERAL_BUS_H__

const gchar peripheral_data_xml[] =
	"<node>"
	"  <interface name='org.tizen.system.peripheral_io'>"
	"    <method name='gpio'>"
	"      <arg type='s' name='function' direction='in'/>"
	"      <arg type='i' name='pin' direction='in'/>"
	"      <arg type='i' name='dir' direction='in'/>"
	"      <arg type='i' name='edge' direction='in'/>"
	"      <arg type='i' name='value' direction='in' />"
	"      <arg type='i' name='re_pin' direction='out'/>"
	"      <arg type='i' name='re_drive' direction='out'/>"
	"      <arg type='i' name='re_edge' direction='out'/>"
	"      <arg type='i' name='re_value' direction='out' />" // read value
	"      <arg type='i' name='re_status' direction='out' />" // return value
	"    </method>"
	"    <method name='i2c'>"
	"      <arg type='s' name='function' direction='in' />"
	"      <arg type='i' name='value' direction='in' />"
	"      <arg type='i' name='fd' direction='in' />"
	"      <arg type='ay' name='data' direction='in' />"
	"      <arg type='i' name='addr' direction='in' />"
	"      <arg type='i' name='re_fd' direction='out' />"
	"      <arg type='ay' name='re_data' direction='out' />"
	"      <arg type='i' name='re_status' direction='out' />" //return value
	"    </method>"
	"    <method name='pwm'>"
	"      <arg type='s' name='function' direction='in' />"
	"      <arg type='i' name='device' direction='in' />"
	"      <arg type='i' name='channel' direction='in' />"
	"      <arg type='i' name='period' direction='in' />"
	"      <arg type='i' name='duty_cycle' direction='in' />"
	"      <arg type='i' name='enabled' direction='in' />"
	"      <arg type='i' name='re_period' direction='out' />"
	"      <arg type='i' name='re_duty' direction='out' />"
	"      <arg type='i' name='re_status' direction='out' />" //return value
	"    </method>"
	"  </interface>"
	"</node>";
#endif /* __PERIPHERAL_BUS_H__ */
