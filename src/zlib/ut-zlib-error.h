#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new Zlib error with [description].
///
/// !return-ref
/// !return-type UtZlibError
UtObject *ut_zlib_error_new(const char *description);

/// Returns [true] if [object] is a [UtZlibError].
bool ut_object_is_zlib_error(UtObject *object);
