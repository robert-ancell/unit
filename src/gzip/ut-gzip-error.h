#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new GZIP error with [description].
///
/// !return-ref
/// !return-type UtGzipError
UtObject *ut_gzip_error_new(const char *description);

/// Returns [true] if [object] is a [UtGzipError].
bool ut_object_is_gzip_error(UtObject *object);
