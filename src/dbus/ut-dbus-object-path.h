#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus object path with [value], e.g. "/org/freedesktop/DBus".
///
/// !return-ref
/// !return-type UtDbusObjectPath
UtObject *ut_dbus_object_path_new(const char *value);

/// Returns the path in this object path, e.g. "/org/freedesktop/DBus".
const char *ut_dbus_object_path_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtDbusObjectPath].
bool ut_object_is_dbus_object_path(UtObject *object);
