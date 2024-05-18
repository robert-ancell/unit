#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new object pair.
///
/// !arg-type item0 Object
/// !arg-type item1 Object
/// !return-ref
/// !return-type UtOrderedHashTable
UtObject *ut_object_pair_new(UtObject *item0, UtObject *item1);

/// Returns [true] if [object] is a [UtObjectPair].
bool ut_object_is_object_pair(UtObject *object);
