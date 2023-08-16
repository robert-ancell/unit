#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*encode_value)(UtObject *object, UtObject *type, UtObject *value);
  UtObject *(*get_error)(UtObject *object);
} UtAsn1EncoderInterface;

extern int ut_asn1_encoder_id;

/// Encode [value]. [type] is an [UtAsn1Type].
void ut_asn1_encoder_encode_value(UtObject *object, UtObject *type,
                                  UtObject *value);

/// Gets any error that occurred during encoding.
UtObject *ut_asn1_encoder_get_error(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1Decoder].
bool ut_object_implements_asn1_encoder(UtObject *object);
