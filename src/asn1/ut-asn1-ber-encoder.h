#include <stdbool.h>
#include <stdint.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1BerEncoder].
///
/// !return-ref
UtObject *ut_asn1_ber_encoder_new();

size_t ut_asn1_ber_encoder_encode_primitive_identifier(UtObject *object,
                                                       UtAsn1TagClass class,
                                                       uint32_t number);

size_t ut_asn1_ber_encoder_encode_constructed_identifier(UtObject *object,
                                                         UtAsn1TagClass class,
                                                         uint32_t number);

size_t ut_asn1_ber_encoder_encode_definite_length(UtObject *object,
                                                  size_t length);

size_t ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value);

size_t ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value);

size_t ut_asn1_ber_encoder_encode_bit_string(UtObject *object, UtObject *value);

size_t ut_asn1_ber_encoder_encode_octet_string(UtObject *object,
                                               UtObject *value);

size_t ut_asn1_ber_encoder_encode_null(UtObject *object);

size_t ut_asn1_ber_encoder_encode_object_identifier(UtObject *object,
                                                    UtObject *identifier);

size_t ut_asn1_ber_encoder_encode_object_descriptor(UtObject *object,
                                                    const char *descriptor);

size_t ut_asn1_ber_encoder_encode_real(UtObject *object, double value);

size_t ut_asn1_ber_encoder_encode_enumerated(UtObject *object, int64_t value);

size_t ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                              UtObject *value);

size_t ut_asn1_ber_encoder_encode_relative_oid(UtObject *object,
                                               UtObject *identifier);

size_t ut_asn1_ber_encoder_encode_numeric_string(UtObject *object,
                                                 const char *value);

size_t ut_asn1_ber_encoder_encode_printable_string(UtObject *object,
                                                   const char *value);

size_t ut_asn1_ber_encoder_encode_ia5_string(UtObject *object,
                                             const char *value);

size_t ut_asn1_ber_encoder_encode_utc_time(UtObject *object, UtObject *value);

size_t ut_asn1_ber_encoder_encode_generalized_time(UtObject *object,
                                                   UtObject *value);

size_t ut_asn1_ber_encoder_encode_graphic_string(UtObject *object,
                                                 const char *value);

size_t ut_asn1_ber_encoder_encode_visible_string(UtObject *object,
                                                 const char *value);

size_t ut_asn1_ber_encoder_encode_general_string(UtObject *object,
                                                 const char *value);

size_t ut_asn1_ber_encoder_encode_bmp_string(UtObject *object,
                                             const char *value);

/// Valid until something else is encoded.
/// !return-ref
UtObject *ut_asn1_ber_encoder_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerEncoder].
bool ut_object_is_asn1_ber_encoder(UtObject *object);
