#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Creates a new RGBA buffer with dimensions [width]x[height] containing 8 bit
/// samples.
///
/// !return-ref
/// !return-type UtRgba8888Buffer
UtObject *ut_rgba8888_buffer_new(size_t width, size_t height);

/// Returns [true] if [object] is a [UtRgba8888Buffer].
bool ut_object_is_rgba8888_buffer(UtObject *object);
