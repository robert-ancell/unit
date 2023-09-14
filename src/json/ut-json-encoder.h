#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new JSON encoder.
///
/// !return-type UtJsonEncoder
/// !return-ref
UtObject *ut_json_encoder_new();

/// Returns a JSON encoded representation of [message].
/// Message can be a combination of [UtString], [UtInt64], [UtFloat64], [UtMap],
/// [UtList], [UtBoolean], [UtNull].
///
/// !return-type UtJsonEncoder
/// !return-ref
char *ut_json_encoder_encode(UtObject *object, UtObject *message);

/// Returns [true] if [object] is a [UtJsonEncoder].
bool ut_object_is_json_encoder(UtObject *object);
