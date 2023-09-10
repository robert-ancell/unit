#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_dbus_variant_new(UtObject *value);

UtObject *ut_dbus_variant_new_take(UtObject *value);

const char *ut_dbus_variant_get_signature(UtObject *object);

UtObject *ut_dbus_variant_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtDbusVariant].
bool ut_object_is_dbus_variant(UtObject *object);
