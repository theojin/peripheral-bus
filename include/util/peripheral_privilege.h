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

#ifndef __PRIVILEGE_CHECKER_H__
#define __PRIVILEGE_CHECKER_H__

#include <gio/gio.h>

void peripheral_privilege_init(void);
void peripheral_privilege_deinit(void);
int peripheral_privilege_check(GDBusMethodInvocation *invocation, GDBusConnection *connection);

#endif /* __PRIVILEGE_CHECKER_H__ */
