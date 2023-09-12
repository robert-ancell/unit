#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GIF error with [description].
///
/// !return-ref
/// !return-type UtGifError
UtObject *ut_gif_error_new(const char *description);

/// Returns [true] if [object] is a [UtGifError].
bool ut_object_is_gif_error(UtObject *object);
