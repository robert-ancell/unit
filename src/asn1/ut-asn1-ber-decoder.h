#include <stdbool.h>
#include <stdint.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1BerDecoder] to decode [data].
///
/// !return-ref
UtObject *ut_asn1_ber_decoder_new(UtObject *data);

UtAsn1TagClass ut_asn1_ber_decoder_get_class(UtObject *object);

bool ut_asn1_ber_decoder_get_constructed(UtObject *object);

unsigned int ut_asn1_ber_decoder_get_number(UtObject *object);

UtObject *ut_asn1_ber_decoder_get_contents(UtObject *object);

bool ut_asn1_ber_decoder_decode_boolean(UtObject *object);

int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object);

char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerDecoder].
bool ut_object_is_asn1_ber_decoder(UtObject *object);
