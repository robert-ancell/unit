#include <stdbool.h>
#include <stdint.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1BerEncoder].
///
/// !return-ref
UtObject *ut_asn1_ber_encoder_new();

void ut_asn1_ber_encoder_encode_primitive_identifier(UtObject *object,
                                                     UtAsn1TagClass class,
                                                     unsigned int number);

void ut_asn1_ber_encoder_encode_constructed_identifier(UtObject *object,
                                                       UtAsn1TagClass class,
                                                       unsigned int number,
                                                       size_t length);

void ut_asn1_ber_encoder_encode_constructed_identifier_indefinite_length(
    UtObject *object, UtAsn1TagClass class, unsigned int number);

void ut_asn1_ber_encoder_encode_end_of_contents(UtObject *object);

void ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value);

void ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value);

void ut_asn1_ber_encoder_encode_octet_string(UtObject *object, UtObject *value);

void ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                            const char *value);

UtObject *ut_asn1_ber_encoder_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerEncoder].
bool ut_object_is_asn1_ber_encoder(UtObject *object);
