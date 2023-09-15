#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new object that contains a single unsigned 256 bit [value].
///
/// !return-type UtUint256
/// !return-ref
UtObject *ut_uint256_new(uint64_t value);

/// Creates a new object that contains a single unsigned 256 bit from the
/// supplied 32 8 bit values.
///
/// !return-type UtUint256
/// !return-ref
UtObject *ut_uint256_new_from_data(
    uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5,
    uint8_t v6, uint8_t v7, uint8_t v8, uint8_t v9, uint8_t v10, uint8_t v11,
    uint8_t v12, uint8_t v13, uint8_t v14, uint8_t v15, uint8_t v16,
    uint8_t v17, uint8_t v18, uint8_t v19, uint8_t v20, uint8_t v21,
    uint8_t v22, uint8_t v23, uint8_t v24, uint8_t v25, uint8_t v26,
    uint8_t v27, uint8_t v28, uint8_t v29, uint8_t v30, uint8_t v31);

/// Creates a new object that contains a single unsigned 256 bit from the
/// supplied 32 8 bit values in [list].
///
/// !arg-type list UtUint8List
/// !return-type UtUint256
/// !return-ref
UtObject *ut_uint256_new_from_uint8_list(UtObject *list);

/// Copy this value.
///
/// !return-type UtUint256
/// !return-ref
UtObject *ut_uint256_copy(UtObject *object);

/// Returns true if this value fits inside an unsigned 64 bit value.
bool ut_uint256_is_uint64(UtObject *object);

/// Returns this value as an unsigned 64 bit value.
/// If the value is greater that 64 bits the remaining bits are truncated.
uint64_t ut_uint256_to_uint64(UtObject *object);

/// Returns the value as 32 unsigned 8 bit values.
///
/// !return-ref
/// !return-type Uint8List
UtObject *ut_uint256_to_uint8_list(UtObject *object);

/// Set this number to [value].
///
/// !arg-type value UtUint256
void ut_uint256_set(UtObject *object, UtObject *value);

/// Modify this number with a boolean and operation using [value].
///
/// !arg-type value UtUint256
void ut_uint256_and(UtObject *object, UtObject *value);

/// Modify this number with a boolean or operation using [value].
///
/// !arg-type value UtUint256
void ut_uint256_or(UtObject *object, UtObject *value);

/// Modify this number with a boolean xor operation using [value].
///
/// !arg-type value UtUint256
void ut_uint256_xor(UtObject *object, UtObject *value);

/// Set this number to [a] + [b].
///
/// !arg-type a UtUint256
/// !arg-type b UtUint256
void ut_uint256_add(UtObject *object, UtObject *a, UtObject *b);

/// Set this number to [a] - [b].
///
/// !arg-type a UtUint256
/// !arg-type b UtUint256
void ut_uint256_sub(UtObject *object, UtObject *a, UtObject *b);

/// Set this number to [a] × [b].
///
/// !arg-type a UtUint256
/// !arg-type b UtUint256
void ut_uint256_mul(UtObject *object, UtObject *a, UtObject *b);

/// Returns [true] if [object] is a [UtUint256].
bool ut_object_is_uint256(UtObject *object);
