#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  float (*get_element)(UtObject *object, size_t index);
  const float *(*get_data)(UtObject *object);
  float *(*get_writable_data)(UtObject *object);
  float *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const float *data,
                 size_t data_length);
} UtFloat32ListInterface;

extern int ut_float32_list_id;

/// Creates a new list that contains 32 bit floating point values.
/// This creates a [UtFloat32Array].
///
/// !return-ref
/// !return-type UtFloat32List
UtObject *ut_float32_list_new();

UtObject *ut_float32_list_new_from_elements(size_t length, ...);

/// Returns the value at [index] of this list.
float ut_float32_list_get_element(UtObject *object, size_t index);

const float *ut_float32_list_get_data(UtObject *object);

float *ut_float32_list_get_writable_data(UtObject *object);

float *ut_float32_list_take_data(UtObject *object);

/// Add [item] to the end of this list.
void ut_float32_list_append(UtObject *object, float item);

void ut_float32_list_append_block(UtObject *object, const float *data,
                                  size_t data_length);

/// Add [item] to the start of this list.
void ut_float32_list_prepend(UtObject *object, float item);

void ut_float32_list_prepend_block(UtObject *object, const float *data,
                                   size_t data_length);

/// Insert [item] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_float32_list_insert(UtObject *object, size_t index, float item);

void ut_float32_list_insert_block(UtObject *object, size_t index,
                                  const float *data, size_t data_length);

/// Returns [true] if [object] is a [UtFloat32List].
bool ut_object_implements_float32_list(UtObject *object);
