#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtAuthCompleteCallback)(UtObject *object, UtObject *error);

UtObject *ut_dbus_auth_server_new(UtObject *input_stream,
                                  UtObject *output_strea);

void ut_dbus_auth_server_run(UtObject *object, UtObject *callback_object,
                             UtAuthCompleteCallback callback);

/// Returns [true] if [object] is a [UtDbusAuthServer].
bool ut_object_is_dbus_auth_server(UtObject *object);
