#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf error with [description].
///
/// !return-ref
/// !return-type UtProtbufError
UtObject *ut_protobuf_error_new(const char *description);

/// Returns [true] if [object] is a [UtProtobufError].
bool ut_object_is_protobuf_error(UtObject *object);
