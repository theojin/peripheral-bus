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

#include <cynara-creds-gdbus.h>
#include <cynara-client.h>
#include <cynara-session.h>

#include "peripheral_privilege.h"
#include "peripheral_log.h"

#define PERIPHERAL_PRIVILEGE "http://tizen.org/privilege/peripheralio"

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

int peripheral_privilege_check(GDBusMethodInvocation *invocation, GDBusConnection *connection)
{
	RETVM_IF(!__cynara, -1, "Cynara does not initialized");

	int ret;
	int pid;
	const char *sender;
	char *session;
	char *client;
	char *user;

	sender = g_dbus_method_invocation_get_sender(invocation);

	cynara_creds_gdbus_get_pid(connection, sender, &pid);
	session = cynara_session_from_pid(pid);

	cynara_creds_gdbus_get_client(connection, sender, CLIENT_METHOD_DEFAULT, &client);
	cynara_creds_gdbus_get_user(connection, sender, USER_METHOD_DEFAULT, &user);

	if (!session || !client || !user) {
		_E("Failed to get client info");
		return -1;
	}

	ret = cynara_check(__cynara, client, session, user, PERIPHERAL_PRIVILEGE);
	if (ret != 0) {
		_E("Failed to check privilege");
		g_free(session);
		g_free(client);
		g_free(user);
		return -EACCES;
	}

	g_free(session);
	g_free(client);
	g_free(user);

	return 0;
}
