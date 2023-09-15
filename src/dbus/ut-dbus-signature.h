#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus signature containing [value], e.g. "a{sv}".
///
/// !return-ref
/// !return-type UtDbusSignature
UtObject *ut_dbus_signature_new(const char *value);

/// Returns the value of this signature, e.g. "a{sv}".
const char *ut_dbus_signature_get_value(UtObject *object);

/// Returns this signature split into separate components, e.g. "ias" -> "i",
/// "as".
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_dbus_signature_split(UtObject *object);

/// Returns [true] if [object] is a [UtDbusSignature].
bool ut_object_is_dbus_signature(UtObject *object);
