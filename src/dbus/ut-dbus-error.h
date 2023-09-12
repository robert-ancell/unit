#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus error with [error_name] and [args].
///
/// !arg-type args UtObjectList NULL
/// !return-ref
/// !return-type UtDBusrror
UtObject *ut_dbus_error_new(const char *error_name, UtObject *args);

/// Returns the name of this error, e.g.
/// "org.freedesktop.DBus.Error.InvalidArgs".
const char *ut_dbus_error_get_error_name(UtObject *object);

/// Returns the arguments with this error.
///
/// !return-type UtObjectList
UtObject *ut_dbus_error_get_args(UtObject *object);

/// Returns [true] if [object] is a [UtDbusError].
bool ut_object_is_dbus_error(UtObject *object);
