#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtNull] object.
/// !return-ref
UtObject *ut_null_new();

/// Returns [true] if [object] is a [UtNull].
bool ut_object_is_null(UtObject *object);
