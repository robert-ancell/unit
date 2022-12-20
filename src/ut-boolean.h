#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtBoolean] object that contains a boolean [value].
UtObject *ut_boolean_new(bool value);

/// Gets the value of [object].
bool ut_boolean_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtBoolean].
bool ut_object_is_boolean(UtObject *object);
