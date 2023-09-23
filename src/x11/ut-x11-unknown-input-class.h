#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 unknown input class of [type].
///
/// !return-ref
/// !return-type UtX11UnknownInputClass
UtObject *ut_x11_unknown_input_class_new(uint16_t type);

/// Returns [true] if [object] is a [UtX11UnknownInputClass].
bool ut_object_is_x11_unknown_input_class(UtObject *object);
