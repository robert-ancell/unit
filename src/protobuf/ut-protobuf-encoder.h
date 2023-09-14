#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf encoder.
///
/// !return-ref
/// !return-type UtProtobufEncoder
UtObject *ut_protobuf_encoder_new();

/// Encode [message] of [type].
///
/// !arg-type type UtProtobufMessageType
/// !arg-type message UtMap
void ut_protobuf_encoder_encode_message(UtObject *object, UtObject *type,
                                        UtObject *message);

/// Returns the data encoded.
/// Valid until something else is encoded.
///
/// !return-type UtUint8List
UtObject *ut_protobuf_encoder_get_data(UtObject *object);

/// Returns the error that occurred during encoding or [NULL] if no error.
///
/// !return-type UtProtobufError NULL
UtObject *ut_protobuf_encoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufEncoder].
bool ut_object_is_protobuf_encoder(UtObject *object);
