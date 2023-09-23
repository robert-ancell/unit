#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new JSON encoder.
///
/// !return-type UtJsonEncoder
/// !return-ref
UtObject *ut_json_encoder_new();

/// Returns a JSON encoded representation of [message].
///
/// !arg-type message UtString
/// !arg-type message UtInt64
/// !arg-type message UtFloat64
/// !arg-type message UtMap
/// !arg-type message UtList
/// !arg-type message UtBoolean
/// !arg-type message UtNull
/// !return-type UtJsonEncoder
/// !return-ref
char *ut_json_encoder_encode(UtObject *object, UtObject *message);

/// Returns [true] if [object] is a [UtJsonEncoder].
bool ut_object_is_json_encoder(UtObject *object);
