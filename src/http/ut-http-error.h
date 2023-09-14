#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new HTTP error with [description].
///
/// !return-ref
/// !return-type UtHttpError
UtObject *ut_http_error_new(const char *description);

/// Returns [true] if [object] is a [UtHttpError].
bool ut_object_is_http_error(UtObject *object);
