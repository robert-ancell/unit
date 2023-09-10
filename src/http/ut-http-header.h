#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtHttpHeader] with [name] and [value].
///
/// !return-ref
UtObject *ut_http_header_new(const char *name, const char *value);

/// Returns the name of the header.
const char *ut_http_header_get_name(UtObject *object);

/// Returns the value of the header.
const char *ut_http_header_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtHttpHeader].
bool ut_object_is_http_header(UtObject *object);
