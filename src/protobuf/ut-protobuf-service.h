#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf service.
///
/// !arg-type method_calls UtObjectList
/// !return-ref
/// !return-type UtProtobufService
UtObject *ut_protobuf_service_new(UtObject *method_calls);

/// Returns the method calls provided by this service.
///
/// !return-type UtObjectList
UtObject *ut_protobuf_service_get_method_calls(UtObject *object);

/// Returns the method call that matches [name] or [NULL] if no method call.
///
/// !return-type UtProtobufMethodCall NULL
UtObject *ut_protobuf_service_lookup_method_call(UtObject *object,
                                                 const char *name);

/// Returns [true] if [object] is a [UtProtobufService].
bool ut_object_is_protobuf_service(UtObject *object);
