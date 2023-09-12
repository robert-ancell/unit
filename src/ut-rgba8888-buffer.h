#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

UtObject *ut_rgba8888_buffer_new(size_t width, size_t height);

/// Returns [true] if [object] is a [UtRgb8888Buffer].
bool ut_object_is_rgba8888_buffer(UtObject *object);
