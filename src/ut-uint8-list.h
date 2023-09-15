#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  uint8_t (*get_element)(UtObject *object, size_t index);
  UtObject *(*get_array)(UtObject *object);
  const uint8_t *(*get_data)(UtObject *object);
  uint8_t *(*get_writable_data)(UtObject *object);
  uint8_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const uint8_t *data,
                 size_t data_length);
  void (*append)(UtObject *object, const uint8_t *data, size_t data_length);
} UtUint8ListInterface;

extern int ut_uint8_list_id;

/// Creates a new list that contains unsigned 8 bit values.
/// This creates a [UtUint8Array].
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_uint8_list_new();

/// Creates a new list that contains unsigned [data_length] 8 bit values from
/// [data]. The data is copied. This creates a [UtUint8Array].
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_uint8_list_new_from_data(const uint8_t *data, size_t data_length);

/// Creates a new list that contains [length] unsigned 8 bit values.
/// This creates a [UtUint8Array].
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_uint8_list_new_from_elements(size_t length, ...);

/// Creates a new list of unsigned 8 bit data in [hex], e.g. "6a54c3".
/// Returns an error if [hex] contains invalid characters or not full 8 bit
/// values.
///
/// !return-type UtUint8List UtError !return-ref
UtObject *ut_uint8_list_new_from_hex_string(const char *hex);

/// Returns the value at [index] of this list.
uint8_t ut_uint8_list_get_element(UtObject *object, size_t index);

/// Returns the value at [index] of this list as a signed 8 bit value.
int8_t ut_uint8_list_get_int8(UtObject *object, size_t index);

/// Returns the two values starting at [index] of this list as an unsigned 16
/// bit value in little endian format.
uint16_t ut_uint8_list_get_uint16_le(UtObject *object, size_t index);

/// Returns the two values starting at [index] of this list as an unsigned 16
/// bit value in big endian format.
uint16_t ut_uint8_list_get_uint16_be(UtObject *object, size_t index);

/// Returns the two values starting at [index] of this list as a signed 16 bit
/// value in little endian format.
int16_t ut_uint8_list_get_int16_le(UtObject *object, size_t index);

/// Returns the two values starting at [index] of this list as a signed 16 bit
/// value in big endian format.
int16_t ut_uint8_list_get_int16_be(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as an unsigned 32
/// bit value in little endian format.
uint32_t ut_uint8_list_get_uint32_le(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as an unsigned 32
/// bit value in big endian format.
uint32_t ut_uint8_list_get_uint32_be(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as a signed 32 bit
/// value in little endian format.
int32_t ut_uint8_list_get_int32_le(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as a signed 32 bit
/// value in big endian format.
int32_t ut_uint8_list_get_int32_be(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as a 32 bit
/// floating point value in little endian format.
float ut_uint8_list_get_float32_le(UtObject *object, size_t index);

/// Returns the four values starting at [index] of this list as a 32 bit
/// floating point value in big endian format.
float ut_uint8_list_get_float32_be(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as an unsigned 64
/// bit value in little endian format.
uint64_t ut_uint8_list_get_uint64_le(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as an unsigned 64
/// bit value in big endian format.
uint64_t ut_uint8_list_get_uint64_be(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as a signed 64 bit
/// value in little endian format.
int64_t ut_uint8_list_get_int64_le(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as a signed 64 bit
/// value in big endian format.
int64_t ut_uint8_list_get_int64_be(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as a 64 bit
/// floating point value in little endian format.
double ut_uint8_list_get_float64_le(UtObject *object, size_t index);

/// Returns the eight values starting at [index] of this list as a 64 bit
/// floating point value in little endian format.
double ut_uint8_list_get_float64_be(UtObject *object, size_t index);

/// Convert this list into an array, so the raw data can be accessed.
/// If this list is already an array no conversion is performed.
///
/// !return-type UtUint8Array.
/// !return-ref
UtObject *ut_uint8_list_get_array(UtObject *object);

/// Returns the memory used for this list or [NULL] if this not supported.
/// Use [ut_uint8_list_get_array] if you must have access to the list memory.
const uint8_t *ut_uint8_list_get_data(UtObject *object);

/// Returns the memory used to write to this this list or [NULL] if this not
/// supported.
uint8_t *ut_uint8_list_get_writable_data(UtObject *object);

/// Returns a copied block of memory containing the contents of this list.
/// When no longer required used [free] to release the memory.
uint8_t *ut_uint8_list_copy_data(UtObject *object);

/// Returns the memory containing the contents of this list.
/// The list will be empty after this call.
/// If the list supports it, this data will be the data that was used in the
/// list - no copy will be performed. When no longer required used [free] to
/// release the memory.
uint8_t *ut_uint8_list_take_data(UtObject *object);

/// Add [value] to the end of this list.
void ut_uint8_list_append(UtObject *object, uint8_t value);

/// Add array [data] of size [data_length] to the end of this list.
void ut_uint8_list_append_block(UtObject *object, const uint8_t *data,
                                size_t data_length);

/// Append an 8 bit signed [value] to the end of this list.
void ut_uint8_list_append_int8(UtObject *object, int8_t value);

/// Append an 16 bit unsigned [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_uint16_le(UtObject *object, uint16_t value);

/// Append an 16 bit unsigned [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_uint16_be(UtObject *object, uint16_t value);

/// Append an 16 bit signed [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_int16_le(UtObject *object, int16_t value);

/// Append an 16 bit signed [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_int16_be(UtObject *object, int16_t value);

/// Append an 32 bit unsigned [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_uint32_le(UtObject *object, uint32_t value);

/// Append an 32 bit unsigned [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_uint32_be(UtObject *object, uint32_t value);

/// Append an 32 bit signed [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_int32_le(UtObject *object, int32_t value);

/// Append an 32 bit signed [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_int32_be(UtObject *object, int32_t value);

/// Append an 32 bit floating point [value] in little endian format to the end
/// of this list.
void ut_uint8_list_append_float32_le(UtObject *object, float value);

/// Append an 32 bit floating point [value] in big endian format to the end of
/// this list.
void ut_uint8_list_append_float32_be(UtObject *object, float value);

/// Append an 64 bit unsigned [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_uint64_le(UtObject *object, uint64_t value);

/// Append an 64 bit unsigned [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_uint64_be(UtObject *object, uint64_t value);

/// Append an 64 bit signed [value] in little endian format to the end of this
/// list.
void ut_uint8_list_append_int64_le(UtObject *object, int64_t value);

/// Append an 64 bit signed [value] in big endian format to the end of this
/// list.
void ut_uint8_list_append_int64_be(UtObject *object, int64_t value);

/// Append an 64 bit floating point [value] in little endian format to the end
/// of this list.
void ut_uint8_list_append_float64_le(UtObject *object, double value);

/// Append an 64 bit floating point [value] in big endian format to the end of
/// this list.
void ut_uint8_list_append_float64_be(UtObject *object, double value);

/// Add [value] to the start of this list.
void ut_uint8_list_prepend(UtObject *object, uint8_t value);

/// Add array [data] of size [data_length] to the start of this list.
void ut_uint8_list_prepend_block(UtObject *object, const uint8_t *data,
                                 size_t data_length);

/// Insert [value] in this list at position [index]. The remaining elements will
/// be moved down.
void ut_uint8_list_insert(UtObject *object, size_t index, uint8_t value);

/// Insert array [data] of length [data_length] in this list in position
/// [index]. The remaining elements will be moved down.
void ut_uint8_list_insert_block(UtObject *object, size_t index,
                                const uint8_t *data, size_t data_length);

/// Returns a string containing this list in hexadecimal form, e.g. "6a54c3".
/// When no longer required used [free] to release the memory.
char *ut_uint8_list_to_hex_string(UtObject *object);

/// Returns [true] if [object] is a [UtUint8List].
bool ut_object_implements_uint8_list(UtObject *object);
