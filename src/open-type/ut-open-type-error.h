#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new OpenType error with [description].
///
/// !return-ref
/// !return-type UtOpenTypeError
UtObject *ut_open_type_error_new(const char *description);

/// Returns [true] if [object] is a [UtOpenTypeError].
bool ut_object_is_open_type_error(UtObject *object);
