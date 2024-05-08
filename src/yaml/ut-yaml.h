#include "ut-object.h"

#pragma once

/// Returns a YAML encoded representation of [message].
/// Message can be a combination of [UtString], [UtInt64], [UtFloat64], [UtMap],
/// [UtList], [UtBoolean], [UtNull].
///
/// !return-ref
char *ut_yaml_encode(UtObject *object);

/// Decode YAML encoded [text].
/// The returned value will be a combination of [UtString], [UtInt64],
/// [UtFloat64], [UtMap], [UtList], [UtBoolean], [UtNull].
///
/// !return-type UtString UtInt64 UtFloat64 UtMap UtList UtBoolean UtNull
/// !return-ref
UtObject *ut_yaml_decode(const char *text);
