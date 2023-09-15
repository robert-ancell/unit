#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new string that contains [text].
/// [text] must be valid memory for the lifetime of this object.
///
/// !return-type UtConstantUtf8String
/// !return-ref
UtObject *ut_constant_utf8_string_new(const char *text);

/// Returns [true] if [object] is a [UtConstantUtf8String].
bool ut_object_is_constant_utf8_string(UtObject *object);
