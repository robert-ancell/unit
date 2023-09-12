#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  uint16_t (*get_element)(UtObject *object, size_t index);
  const uint16_t *(*get_data)(UtObject *object);
  uint16_t *(*get_writable_data)(UtObject *object);
  uint16_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const uint16_t *data,
                 size_t data_length);
} UtUint16ListInterface;

extern int ut_uint16_list_id;

/// Creates a new list that contains unsigned 16 bit values.
/// This creates a [UtUint16Array].
///
/// !return-type UtUint16List
/// !return-ref
UtObject *ut_uint16_list_new();

UtObject *ut_uint16_list_new_from_elements(size_t length, ...);

UtObject *ut_uint16_list_new_from_hex_string(const char *hex);

/// Returns the value at [index] of this list.
uint16_t ut_uint16_list_get_element(UtObject *object, size_t index);

const uint16_t *ut_uint16_list_get_data(UtObject *object);

uint16_t *ut_uint16_list_get_writable_data(UtObject *object);

/// Returns the memory containing the contents of this list.
/// The list will be empty after this call.
/// If the list supports it, this data will be the data that was used in the
/// list - no copy will be performed. When no longer required used [free] to
/// release the memory.
uint16_t *ut_uint16_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_uint16_list_append(UtObject *object, uint16_t item);

/// Add array [data] of size [data_length] to the end of this list.
void ut_uint16_list_append_block(UtObject *object, const uint16_t *data,
                                 size_t data_length);

/// Add [value] to the start of this list.
void ut_uint16_list_prepend(UtObject *object, uint16_t item);

/// Add array [data] of size [data_length] to the start of this list.
void ut_uint16_list_prepend_block(UtObject *object, const uint16_t *data,
                                  size_t data_length);

void ut_uint16_list_insert(UtObject *object, size_t index, uint16_t item);

void ut_uint16_list_insert_block(UtObject *object, size_t index,
                                 const uint16_t *data, size_t data_length);

char *ut_uint16_list_to_hex_string(UtObject *object);

/// Returns [true] if [object] is a [UtUint16List].
bool ut_object_implements_uint16_list(UtObject *object);
