#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new PNG error with [description].
///
/// !return-ref
/// !return-type UtPngError
UtObject *ut_png_error_new(const char *description);

/// Returns [true] if [object] is a [UtPngError].
bool ut_object_is_png_error(UtObject *object);
