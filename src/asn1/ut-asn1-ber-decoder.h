#include <stdbool.h>
#include <stdint.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1BerDecoder] to decode [data].
///
/// !return-ref
UtObject *ut_asn1_ber_decoder_new(UtObject *data);

uint32_t ut_asn1_ber_decoder_get_identifier_number(UtObject *object);

UtAsn1TagClass ut_asn1_ber_decoder_get_tag_class(UtObject *object);

bool ut_asn1_ber_decoder_get_constructed(UtObject *object);

size_t ut_asn1_ber_decoder_get_length(UtObject *object);

UtObject *ut_asn1_ber_decoder_get_contents(UtObject *object);

UtObject *ut_asn1_ber_decoder_get_error(UtObject *object);

bool ut_asn1_ber_decoder_decode_boolean(UtObject *object);

int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object);

void ut_asn1_ber_decoder_decode_null(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_object_identifier(UtObject *object);

int64_t ut_asn1_ber_decoder_decode_enumerated(UtObject *object);

char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object);

UtObject *
ut_asn1_ber_decoder_decode_relative_object_identifier(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_sequence(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_set(UtObject *object);

char *ut_asn1_ber_decoder_decode_numeric_string(UtObject *object);

char *ut_asn1_ber_decoder_decode_printable_string(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_ia5_string(UtObject *object);

UtObject *ut_asn1_ber_decoder_decode_utc_time(UtObject *object);

char *ut_asn1_ber_decoder_decode_visible_string(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerDecoder].
bool ut_object_is_asn1_ber_decoder(UtObject *object);
