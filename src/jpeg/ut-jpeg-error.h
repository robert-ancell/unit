#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_jpeg_error_new(const char *description);

/// Returns [true] if [object] is a [UtJpegError].
bool ut_object_is_jpeg_error(UtObject *object);
