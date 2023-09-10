#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzw_error_new(const char *description);

/// Returns [true] if [object] is a [UtLzwError].
bool ut_object_is_lzw_error(UtObject *object);
