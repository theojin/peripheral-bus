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

#include <stdint.h>
#include <string.h>
#include <gio/gio.h>

#include "peripheral_bus.h"
#include "peripheral_common.h"

int peripheral_bus_get_client_info(GDBusMethodInvocation *invocation, peripheral_bus_s *pb_data, pb_client_info_s *client_info)
{
	guint pid = 0;
	GError *error = NULL;
	GVariant *_ret;
	const gchar *id;

	id = g_dbus_method_invocation_get_sender(invocation);
	_ret = g_dbus_connection_call_sync(pb_data->connection,
		"org.freedesktop.DBus",
		"/org/freedesktop/DBus",
		"org.freedesktop.DBus",
		"GetConnectionUnixProcessID",
		g_variant_new("(s)", id),
		NULL,
		G_DBUS_CALL_FLAGS_NONE,
		-1,
		NULL,
		&error);

	if (_ret == NULL) {
		_E("Failed to get client pid, %s", error->message);
		g_error_free(error);

		return -1;
	}

	g_variant_get(_ret, "(u)", &pid);
	g_variant_unref(_ret);

	client_info->pid = (pid_t)pid;
	client_info->pgid = getpgid(pid);
	client_info->id = strdup(id);

	return 0;
}

GVariant *peripheral_bus_build_variant_ay(uint8_t *data, int length)
{
	GVariantBuilder *builder;
	GVariant *variant;
	int i;

	if (data == NULL)
		return NULL;

	builder = g_variant_builder_new(G_VARIANT_TYPE("a(y)"));

	for (i = 0; i < length; i++)
		g_variant_builder_add(builder, "(y)", data[i]);

	variant = g_variant_new("a(y)", builder);
	g_variant_builder_unref(builder);

	return variant;
}
