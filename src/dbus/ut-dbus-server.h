#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_dbus_server_new();

void ut_dbus_server_listen_unix(UtObject *object, const char *path);

bool ut_object_is_dbus_server(UtObject *object);
