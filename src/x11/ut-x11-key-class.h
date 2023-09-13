#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_key_class_new(UtObject *keys);

/// Returns [true] if [object] is a [UtX11KeyClass].
bool ut_object_is_x11_key_class(UtObject *object);
