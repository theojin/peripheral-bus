/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <sys/types.h>
#include <unistd.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include <peripheral_io.h>

#include <cynara-creds-gdbus.h>
#include <cynara-client.h>
#include <cynara-session.h>

#include "peripheral_bus.h"
#include "peripheral_log.h"

#define CACHE_SIZE  100

static cynara *__cynara;

void peripheral_privilege_init(void)
{
	int err;
	cynara_configuration* conf = NULL;

	err = cynara_configuration_create(&conf);
	RETM_IF(err != CYNARA_API_SUCCESS, "Failed to create cynara configuration");

	err = cynara_configuration_set_cache_size(conf, CACHE_SIZE);
	if (err != CYNARA_API_SUCCESS) {
		_E("Failed to set cynara cache size");
		cynara_configuration_destroy(conf);
		return;
	}

	err = cynara_initialize(&__cynara, conf);
	cynara_configuration_destroy(conf);
	if (err != CYNARA_API_SUCCESS) {
		_E("Failed to initialize cynara");
		__cynara = NULL;
		return;
	}

	_D("Cynara initialized");
}

void peripheral_privilege_deinit(void)
{
	if (__cynara)
		cynara_finish(__cynara);

	_D("Cynara deinitialized");
}

bool peripheral_privilege_check(const char* client, const char* session, const char* user, const char* privilege)
{
	RETVM_IF(!privilege, true, "Invalid parameter");
	RETVM_IF(!__cynara, false, "Cynara does not initialized");

	int ret = cynara_check(__cynara, client, session, user, privilege);
	return (ret == CYNARA_API_ACCESS_ALLOWED);
}
