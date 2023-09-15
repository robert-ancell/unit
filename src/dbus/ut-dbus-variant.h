#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus variant containing [value].
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtDbusVariant
UtObject *ut_dbus_variant_new(UtObject *value);

/// Creates a new DBus variant containing [value].
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtDbusVariant
UtObject *ut_dbus_variant_new_take(UtObject *value);

/// Returns the value inside this variant.
///
/// !return-type UtObject
UtObject *ut_dbus_variant_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtDbusVariant].
bool ut_object_is_dbus_variant(UtObject *object);
