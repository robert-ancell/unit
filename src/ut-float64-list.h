#include <stdarg.h>
#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  double (*get_element)(UtObject *object, size_t index);
  const double *(*get_data)(UtObject *object);
  double *(*get_writable_data)(UtObject *object);
  double *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const double *data,
                 size_t data_length);
} UtFloat64ListInterface;

extern int ut_float64_list_id;

/// Creates a new list that contains 64 bit floating point values.
/// This creates a [UtFloat64Array].
///
/// !return-ref
/// !return-type UtFloat64List
UtObject *ut_float64_list_new();

/// Creates a new list that contains [length] 64 bit floating point values.
/// This creates a [UtFloat64Array].
///
/// !return-ref
/// !return-type UtFloat64List
UtObject *ut_float64_list_new_from_elements(size_t length, ...);

/// Returns the value at [index] of this list.
double ut_float64_list_get_element(UtObject *object, size_t index);

/// Returns the memory used for this list or [NULL] if this not supported.
/// Use [ut_float64_list_get_array] if you must have access to the list memory.
const double *ut_float64_list_get_data(UtObject *object);

/// Returns the memory used to write to this this list or [NULL] if this not
/// supported.
double *ut_float64_list_get_writable_data(UtObject *object);

/// Returns a copied block of memory containing the contents of this list.
/// When no longer required used [free] to release the memory.
double *ut_float64_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_float64_list_append(UtObject *object, double value);

void ut_float64_list_append_block(UtObject *object, const double *data,
                                  size_t data_length);

/// Add [value] to the start of this list.
void ut_float64_list_prepend(UtObject *object, double value);

void ut_float64_list_prepend_block(UtObject *object, const double *data,
                                   size_t data_length);

/// Insert [value] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_float64_list_insert(UtObject *object, size_t index, double value);

void ut_float64_list_insert_block(UtObject *object, size_t index,
                                  const double *data, size_t data_length);

/// Returns [true] if [object] is a [UtFloat64List].
bool ut_object_implements_float64_list(UtObject *object);
