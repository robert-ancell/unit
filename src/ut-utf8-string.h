#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new UTF-8 string containing [text].
///
/// !return-ref
/// !return-type UtUtf8String
UtObject *ut_utf8_string_new(const char *text);

/// Creates a new UTF-8 string containing the first [length] characters of
/// [text].
///
/// !return-ref
/// !return-type UtUtf8String
UtObject *ut_utf8_string_new_sized(const char *text, size_t length);

/// Creates a new UTF-8 string from [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtUtf8String
UtObject *ut_utf8_string_new_from_data(UtObject *data);

/// Returns [true] if [object] is a [UtUtf8String].
bool ut_object_is_utf8_string(UtObject *object);
