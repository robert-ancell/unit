#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ordered hash table.
///
/// !return-ref
/// !return-type UtOrderedHashTable
UtObject *ut_ordered_hash_table_new();

/// Returns [true] if [object] is a [UtOrderedHashTable].
bool ut_object_is_ordered_hash_table(UtObject *object);
