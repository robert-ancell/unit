#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  bool (*get_element)(UtObject *object, size_t index);
  void (*insert)(UtObject *object, size_t index, bool value);
} UtBooleanListInterface;

extern int ut_boolean_list_id;

/// Creates a new list that contains boolean values.
/// This creates a [UtBooleanArray].
///
/// !return-ref
/// !return-type UtBooleanList
UtObject *ut_boolean_list_new();

/// Returns the value at [index] of this list.
bool ut_boolean_list_get_element(UtObject *object, size_t index);

/// Add [value] to the end of this list.
void ut_boolean_list_append(UtObject *object, bool value);

/// Add [value] to the start of this list.
void ut_boolean_list_prepend(UtObject *object, bool value);

/// Insert [value] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_boolean_list_insert(UtObject *object, size_t index, bool value);

/// Returns [true] if [object] is a [UtBooleanList].
bool ut_object_implements_boolean_list(UtObject *object);
