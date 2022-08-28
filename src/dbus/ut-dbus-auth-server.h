#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtAuthCompleteCallback)(void *user_data, UtObject *error);

UtObject *ut_dbus_auth_server_new(UtObject *input_stream,
                                  UtObject *output_strea);

void ut_dbus_auth_server_run(UtObject *object, UtAuthCompleteCallback callback,
                             void *user_data, UtObject *cancel);

bool ut_object_is_dbus_auth_server(UtObject *object);
