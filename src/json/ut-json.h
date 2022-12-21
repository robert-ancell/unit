#include "ut-object.h"

#pragma once

/// Returns a JSON encoded representation of [message].
/// Message can be a combination of [UtString], [UtInt64], [UtFloat64], [UtMap],
/// [UtList], [UtBoolean], [UtNull]. !return-ref
char *ut_json_encode(UtObject *object);

/// Decode JSON encoded [text].
/// The returned value will be a combination of [UtString], [UtInt64],
/// [UtFloat64], [UtMap], [UtList], [UtBoolean], [UtNull]. !return-ref
UtObject *ut_json_decode(const char *text);
