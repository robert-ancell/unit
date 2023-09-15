#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtAuthCompleteCallback)(UtObject *object, UtObject *error);

/// Creates a new DBus authentication server to read requests from
/// [input_stream] and write to [output_stream].
///
/// !arg-type input_stream UtInputStream
/// !arg-type output_stream UtOutputStream
/// !return-ref
/// !return-type UtDbusAuthServer
UtObject *ut_dbus_auth_server_new(UtObject *input_stream,
                                  UtObject *output_stream);

/// Start the authentication server and call [callback] when authentication is
/// complete.
void ut_dbus_auth_server_run(UtObject *object, UtObject *callback_object,
                             UtAuthCompleteCallback callback);

/// Returns [true] if [object] is a [UtDbusAuthServer].
bool ut_object_is_dbus_auth_server(UtObject *object);
