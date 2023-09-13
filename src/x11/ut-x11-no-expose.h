#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 no expose event.
///
/// !return-ref
/// !return-type UtX11NoExpose
UtObject *ut_x11_no_expose_new();

/// Returns [true] if [object] is a [UtX11NoExpose].
bool ut_object_is_x11_no_expose(UtObject *object);
