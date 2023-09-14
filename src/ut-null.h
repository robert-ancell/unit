#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new null object.
///
/// !return-ref
/// !return-type UtNull
UtObject *ut_null_new();

/// Returns [true] if [object] is a [UtNull].
bool ut_object_is_null(UtObject *object);
