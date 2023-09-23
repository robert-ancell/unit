#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a DBus struct that contains a [NULL] terminated list of values
/// starting at [value0].
///
/// !arg-type value0 UtObject
/// !return-ref
/// !return-type UtDbusStruct
UtObject *ut_dbus_struct_new(UtObject *value0, ...);

/// Creates a DBus struct that contains a [NULL] terminated list of values
/// starting at [value0].
///
/// !arg-type value0 UtObject
/// !return-ref
/// !return-type UtDbusStruct
UtObject *ut_dbus_struct_new_take(UtObject *value0, ...);

/// Creates a DBus struct that contains [values].
///
/// !arg-type values UtObjectList
/// !return-ref
/// !return-type UtDbusStruct
UtObject *ut_dbus_struct_new_from_list(UtObject *values);

/// Returns the value in position [index].
///
/// !return-type UtObject
UtObject *ut_dbus_struct_get_value(UtObject *object, size_t index);

/// Returns the values in this struct.
///
/// !return-type UtObjectList
UtObject *ut_dbus_struct_get_values(UtObject *object);

/// Returns [true] if [object] is a [UtDbusStruct].
bool ut_object_is_dbus_struct(UtObject *object);
