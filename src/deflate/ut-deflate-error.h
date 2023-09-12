#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new deflate error with [description].
///
/// !return-ref
/// !return-type UtDeflateError
UtObject *ut_deflate_error_new(const char *description);

/// Returns [true] if [object] is a [UtDeflateError].
bool ut_object_is_deflate_error(UtObject *object);
