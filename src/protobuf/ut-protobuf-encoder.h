#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufEncoder].
///
/// !return-ref
UtObject *ut_protobuf_encoder_new();

/// Encode [message] of [type].
void ut_protobuf_encoder_encode_message(UtObject *object, UtObject *type,
                                        UtObject *message);

/// Valid until something else is encoded.
UtObject *ut_protobuf_encoder_get_data(UtObject *object);

/// Returns the error that occurred during encoding or NULL if no error.
UtObject *ut_protobuf_encoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufEncoder].
bool ut_object_is_protobuf_encoder(UtObject *object);
