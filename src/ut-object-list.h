#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  UtObject *(*get_element)(UtObject *object, size_t index);
} UtObjectListInterface;

extern int ut_object_list_id;

/// Creates a new list that contains objects.
/// This creates a [UtObjectArray].
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_object_list_new();

/// Returns the value at [index] of this list.
///
/// !return-ref
/// !return-type UtObject
UtObject *ut_object_list_get_element(UtObject *object, size_t index);

/// Returns [true] if [object] is a [UtObjectList].
bool ut_object_implements_object_list(UtObject *object);
