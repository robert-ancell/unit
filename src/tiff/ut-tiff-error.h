#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_tiff_error_new(const char *description);

/// Returns [true] if [object] is a [UtTiffError].
bool ut_object_is_tiff_error(UtObject *object);
