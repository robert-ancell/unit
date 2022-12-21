#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUint32] object that contains [value].
///
/// !return-ref
UtObject *ut_uint32_new(uint32_t value);

/// Gets the value of [object].
uint32_t ut_uint32_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtUint32].
bool ut_object_is_uint32(UtObject *object);
