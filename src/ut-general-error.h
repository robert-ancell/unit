#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new general error with [description].
///
/// !return-ref
/// !return-type UtGeneralError
UtObject *ut_general_error_new(const char *description);

/// Creates a new general error with [description].
/// [description] must be allocated for this call, e.g. with
/// [ut_cstring_new_printf].
///
/// !return-ref
/// !return-type UtGeneralError
UtObject *ut_general_error_new_take(char *description);

/// Returns [true] if [object] is a [UtGeneralError].
bool ut_object_is_general_error(UtObject *object);
