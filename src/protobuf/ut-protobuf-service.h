#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufService] object.
///
/// !return-ref
UtObject *ut_protobuf_service_new(UtObject *method_calls);

UtObject *ut_protobuf_service_get_method_calls(UtObject *object);

UtObject *ut_protobuf_service_get_method_call(UtObject *object,
                                              const char *name);

/// Returns [true] if [object] is a [UtProtobufService].
bool ut_object_is_protobuf_service(UtObject *object);
