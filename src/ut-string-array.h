#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

/// Creates a new array that contains strings.
///
/// !return-ref
/// !return-type UtStringArray
UtObject *ut_string_array_new();

/// Creates a new array from the [NULL] terminated list of strings [value].
///
/// !return-ref
/// !return-type UtStringArray
UtObject *ut_string_array_new_from_elements(const char *value, ...);

/// Creates a new array from the list of strings [value].
///
/// !return-ref
/// !return-type UtStringArray
UtObject *ut_string_array_new_from_va_elements(const char *value, va_list ap);

/// Returns [true] if [object] is a [UtStringArray].
bool ut_object_is_string_array(UtObject *object);
