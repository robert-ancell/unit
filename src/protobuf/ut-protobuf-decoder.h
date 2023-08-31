#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtProtobufDecoder] to decode [data].
///
/// !return-ref
UtObject *ut_protobuf_decoder_new(UtObject *data);

/// Decode message of [type]. [type] is an [UtProtobufMessageType].
UtObject *ut_protobuf_decoder_decode_message(UtObject *object, UtObject *type);

/// Gets any error that occurred during decoding.
UtObject *ut_protobuf_decoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufDecoder].
bool ut_object_is_protobuf_decoder(UtObject *object);
