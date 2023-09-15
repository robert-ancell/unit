#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  int32_t (*get_element)(UtObject *object, size_t index);
  const int32_t *(*get_data)(UtObject *object);
  int32_t *(*get_writable_data)(UtObject *object);
  int32_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const int32_t *data,
                 size_t data_length);
} UtInt32ListInterface;

extern int ut_int32_list_id;

/// Creates a new list that contains signed 32 bit values.
/// This creates a [UtInt32Array].
///
/// !return-type UtInt32List
/// !return-ref
UtObject *ut_int32_list_new();

/// Creates a new list that contains from [length] signed 32 bit values.
/// This creates a [UtInt32Array].
///
/// !return-type UtInt32List
/// !return-ref
UtObject *ut_int32_list_new_from_elements(size_t length, ...);

/// Returns the value at [index] of this list.
int32_t ut_int32_list_get_element(UtObject *object, size_t index);

/// Returns the memory used for this list or [NULL] if this not supported.
/// Use [ut_int32_list_get_array] if you must have access to the list memory.
const int32_t *ut_int32_list_get_data(UtObject *object);

/// Returns the memory used to write to this this list or [NULL] if this not
/// supported.
int32_t *ut_int32_list_get_writable_data(UtObject *object);

/// Returns the memory containing the contents of this list.
/// The list will be empty after this call.
/// If the list supports it, this data will be the data that was used in the
/// list - no copy will be performed. When no longer required used [free] to
/// release the memory.
int32_t *ut_int32_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_int32_list_append(UtObject *object, int32_t value);

/// Add array [data] of size [data_length] to the end of this list.
void ut_int32_list_append_block(UtObject *object, const int32_t *data,
                                size_t data_length);

/// Add [value] to the start of this list.
void ut_int32_list_prepend(UtObject *object, int32_t value);

/// Add array [data] of size [data_length] to the start of this list.
void ut_int32_list_prepend_block(UtObject *object, const int32_t *data,
                                 size_t data_length);

/// Insert [value] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_int32_list_insert(UtObject *object, size_t index, int32_t value);

void ut_int32_list_insert_block(UtObject *object, size_t index,
                                const int32_t *data, size_t data_length);

/// Returns [true] if [object] is a [UtInt32List].
bool ut_object_implements_int32_list(UtObject *object);
