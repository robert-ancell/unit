#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new TVG error with [description].
///
/// !return-ref
/// !return-type UtTvgError
UtObject *ut_tvg_error_new(const char *description);

/// Returns [true] if [object] is a [UtTvgError].
bool ut_object_is_tvg_error(UtObject *object);
