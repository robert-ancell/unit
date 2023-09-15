#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus dictionary that has keys that match [key_signature] and
/// values that match [value_signature].
///
/// !return-ref
/// !return-type UtDbusDict
UtObject *ut_dbus_dict_new(const char *key_signature,
                           const char *value_signature);

/// Returns the signature of keys in this dictionary.
const char *ut_dbus_dict_get_key_signature(UtObject *object);

/// Retusn the signature of values in this dictionary.
const char *ut_dbus_dict_get_value_signature(UtObject *object);

/// Returns [true] if [object] is a [UtDbusDict].
bool ut_object_is_dbus_dict(UtObject *object);
