#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  int64_t (*get_element)(UtObject *object, size_t index);
  const int64_t *(*get_data)(UtObject *object);
  int64_t *(*get_writable_data)(UtObject *object);
  int64_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const int64_t *data,
                 size_t data_length);
} UtInt64ListInterface;

extern int ut_int64_list_id;

/// Creates a new list that contains signed 64 bit values.
/// This creates a [UtInt64Array].
///
/// !return-type UtInt64List
/// !return-ref
UtObject *ut_int64_list_new();

UtObject *ut_int64_list_new_from_elements(size_t length, ...);

/// Returns the value at [index] of this list.
int64_t ut_int64_list_get_element(UtObject *object, size_t index);

const int64_t *ut_int64_list_get_data(UtObject *object);

int64_t *ut_int64_list_get_writable_data(UtObject *object);

/// Returns the memory containing the contents of this list.
/// The list will be empty after this call.
/// If the list supports it, this data will be the data that was used in the
/// list - no copy will be performed. When no longer required used [free] to
/// release the memory.
int64_t *ut_int64_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_int64_list_append(UtObject *object, int64_t item);

/// Add array [data] of size [data_length] to the end of this list.
void ut_int64_list_append_block(UtObject *object, const int64_t *data,
                                size_t data_length);

/// Add [value] to the start of this list.
void ut_int64_list_prepend(UtObject *object, int64_t item);

/// Add array [data] of size [data_length] to the start of this list.
void ut_int64_list_prepend_block(UtObject *object, const int64_t *data,
                                 size_t data_length);

void ut_int64_list_insert(UtObject *object, size_t index, int64_t item);

void ut_int64_list_insert_block(UtObject *object, size_t index,
                                const int64_t *data, size_t data_length);

/// Returns [true] if [object] is a [UtInt64List].
bool ut_object_implements_int64_list(UtObject *object);
