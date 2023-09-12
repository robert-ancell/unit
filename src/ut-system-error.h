#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new system error with [code] e.g. EEXIST.
///
/// !return-ref
/// !return-type UtSystemError
UtObject *ut_system_error_new(int code);

/// Returns the error code used in this error.
int ut_system_error_get_code(UtObject *object);

/// Returns [true] if [object] is a [UtSystemError].
bool ut_object_is_system_error(UtObject *object);
