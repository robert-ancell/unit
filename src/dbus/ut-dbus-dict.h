#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_dbus_dict_new(const char *key_signature,
                           const char *value_signature);

const char *ut_dbus_dict_get_key_signature(UtObject *object);

const char *ut_dbus_dict_get_value_signature(UtObject *object);

/// Returns [true] if [object] is a [UtDbusDict].
bool ut_object_is_dbus_dict(UtObject *object);
