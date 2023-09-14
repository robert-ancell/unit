#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf method call.
///
/// !return-ref
/// !return-type UtProtobufMethodCall
UtObject *ut_protobuf_method_call_new(UtObject *request_type,
                                      UtObject *response_type);

UtObject *ut_protobuf_method_call_get_request_type(UtObject *object);

UtObject *ut_protobuf_method_call_get_response_type(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufMethodCall].
bool ut_object_is_protobuf_method_call(UtObject *object);
