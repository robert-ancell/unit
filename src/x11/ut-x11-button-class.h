#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 button class with [state] and [labels].
///
/// !arg-type state UtUint32List
/// !arg-type labels UtUint32List
/// !return-ref
/// !return-type UtX11ButtonClass
UtObject *ut_x11_button_class_new(UtObject *state, UtObject *labels);

/// Returns [true] if [object] is a [UtX11ButtonClass].
bool ut_object_is_x11_button_class(UtObject *object);
