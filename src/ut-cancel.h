#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtCancel] object.
/// !return-ref
UtObject *ut_cancel_new();

/// Marks [object] as cancelled.
void ut_cancel_activate(UtObject *object);

/// Returns [true] if [object] has had [ut_cancel_activate] called.
/// For convenience, this function returns [false] if [object] is [NULL].
bool ut_cancel_is_active(UtObject *object);

/// Returns [true] if [object] is a [UtCancel].
bool ut_object_is_cancel(UtObject *object);
