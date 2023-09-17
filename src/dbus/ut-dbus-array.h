#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus array containing contents that have [value_signature].
///
/// !return-ref
/// !return-type UtDbusArray
UtObject *ut_dbus_array_new(const char *value_signature);

/// Returns the signature for the types in this array.
const char *ut_dbus_array_get_value_signature(UtObject *object);

/// Returns [true] if [object] is a [UtDbusArray].
bool ut_object_is_dbus_array(UtObject *object);
