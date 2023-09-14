#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new protobuf decoder to decode [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtProtobufDecoder
UtObject *ut_protobuf_decoder_new(UtObject *data);

/// Decode message of [type].
///
/// !arg-type type UtProtobufMessageType
UtObject *ut_protobuf_decoder_decode_message(UtObject *object, UtObject *type);

/// Gets any error that occurred during decoding.
UtObject *ut_protobuf_decoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtProtobufDecoder].
bool ut_object_is_protobuf_decoder(UtObject *object);
