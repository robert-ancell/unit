#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 touch class with [mode] that allows [num_touches].
///
/// !return-ref
/// !return-type UtX11TouchClass
UtObject *ut_x11_touch_class_new(uint8_t mode, uint8_t num_touches);

/// Returns [true] if [object] is a [UtX11TouchClass].
bool ut_object_is_x11_touch_class(UtObject *object);
