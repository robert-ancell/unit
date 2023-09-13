#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 key class containing [keys].
///
/// !arg-type keys UtUint32List
/// !return-ref
/// !return-type UtX11KeyClass
UtObject *ut_x11_key_class_new(UtObject *keys);

/// Returns [true] if [object] is a [UtX11KeyClass].
bool ut_object_is_x11_key_class(UtObject *object);
