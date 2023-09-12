#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new LZW error with [description].
///
/// !return-ref
/// !return-type UtLzwError
UtObject *ut_lzw_error_new(const char *description);

/// Returns [true] if [object] is a [UtLzwError].
bool ut_object_is_lzw_error(UtObject *object);
