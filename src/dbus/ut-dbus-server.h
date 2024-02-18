#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus server.
///
/// !return-ref
/// !return-type UtDbusServer
UtObject *ut_dbus_server_new();

/// Start listening on unix socket [path]. If fails returns [false] and sets
/// [error].
bool ut_dbus_server_listen_unix(UtObject *object, const char *path,
                                UtObject **error);

/// Returns [true] if [object] is a [UtDbusServer].
bool ut_object_is_dbus_server(UtObject *object);
