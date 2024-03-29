#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  UtObject *(*decode_value)(UtObject *object, UtObject *type);
  UtObject *(*get_error)(UtObject *object);
} UtAsn1DecoderInterface;

extern int ut_asn1_decoder_id;

/// Decode value of [type].
///
/// !arg-type type UtAsn1Type
/// !return-ref
/// !return-type UtObject
UtObject *ut_asn1_decoder_decode_value(UtObject *object, UtObject *type);

/// Gets any error that occurred during decoding.
///
/// !return-type UtAsn1Error NULL
UtObject *ut_asn1_decoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1Decoder].
bool ut_object_implements_asn1_decoder(UtObject *object);
