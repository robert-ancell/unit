#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new bit list packed most signfificant bit first.
///
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_msb();

/// Creates a new bit list packed least signfificant bit first.
///
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_lsb();

/// Creates a new bit list packed most signfificant bit first.
/// The list is populated from [bit_count] bits in [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_msb_from_data(UtObject *data, size_t bit_count);

/// Creates a new bit list packed least signfificant bit first.
/// The list is populated from [bit_count] bits in [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_lsb_from_data(UtObject *data, size_t bit_count);

/// Creates a new bit list packed most signfificant bit first from [bin]. e.g.
/// "10110011".
///
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_msb_from_bin_string(const char *bin);

/// Creates a new bit list packed least signfificant bit first from [bin]. e.g.
/// "10110011".
///
/// !return-ref
/// !return-type UtBitList
UtObject *ut_bit_list_new_lsb_from_bin_string(const char *bin);

/// Returns the number of bits in this list.
size_t ut_bit_list_get_length(UtObject *object);

/// Returns the value at [index] in this list.
bool ut_bit_list_get_element(UtObject *object, size_t index);

/// Returns the packed data that this bit list is made of.
///
/// !return-type UtUint8List
UtObject *ut_bit_list_get_data(UtObject *object);

/// Adds [value] to the end of this list.
void ut_bit_list_append(UtObject *object, bool value);

/// Adds [bit_count] bits from [data] to the end of this list.
/// The least significant bits of [data] are used.
void ut_bit_list_append_bits(UtObject *object, uint8_t data, size_t bit_count);

/// Adds [value] to the end of this list.
///
/// !arg-type value UtBitList
void ut_bit_list_append_list(UtObject *object, UtObject *value);

/// Returns a binary string representation of this list, e.g. "10110011".
char *ut_bit_list_to_bin_string(UtObject *object);

/// Returns [true] if [object] is a [UtBitList].
bool ut_object_is_bit_list(UtObject *object);
