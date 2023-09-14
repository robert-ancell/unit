#include <stdbool.h>
#include <stdint.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 BER encoder.
///
/// !return-ref
/// !return-type UtAsn1BerEncoder
UtObject *ut_asn1_ber_encoder_new();

/// Encode identifier [class] and [number] for primitive contents.
size_t ut_asn1_ber_encoder_encode_primitive_identifier(UtObject *object,
                                                       UtAsn1TagClass class,
                                                       uint32_t number);

/// Encode identifier [class] and [number] for constructed contents.
size_t ut_asn1_ber_encoder_encode_constructed_identifier(UtObject *object,
                                                         UtAsn1TagClass class,
                                                         uint32_t number);

/// Encode [length] for contents.
size_t ut_asn1_ber_encoder_encode_definite_length(UtObject *object,
                                                  size_t length);

/// Encode [value] as a BOOLEAN.
size_t ut_asn1_ber_encoder_encode_boolean(UtObject *object, bool value);

/// Encode [value] as an INTEGER.
size_t ut_asn1_ber_encoder_encode_integer(UtObject *object, int64_t value);

/// Encode [value] as a BIT STRING.
///
/// !arg-type value UtBitList
size_t ut_asn1_ber_encoder_encode_bit_string(UtObject *object, UtObject *value);

/// Encode [value] as an OCTET STRING.
///
/// !arg-type value UtUint8List
size_t ut_asn1_ber_encoder_encode_octet_string(UtObject *object,
                                               UtObject *value);

/// Encode NULL.
size_t ut_asn1_ber_encoder_encode_null(UtObject *object);

/// Encode [value] as an OBJECT IDENTIFIER.
///
/// !arg-type identifier UtObjectIdentifier
size_t ut_asn1_ber_encoder_encode_object_identifier(UtObject *object,
                                                    UtObject *identifier);

/// Encode [value] as an ObjectDescriptor.
size_t ut_asn1_ber_encoder_encode_object_descriptor(UtObject *object,
                                                    const char *descriptor);

/// Encode [value] as an EXTERNAL.
///
/// !arg-type value UtAsn1EmbeddedValue
size_t ut_asn1_ber_encoder_encode_external(UtObject *object, UtObject *value);

/// Encode [value] as a REAL.
size_t ut_asn1_ber_encoder_encode_real(UtObject *object, double value);

/// Encode [value] as an ENUMERATED.
size_t ut_asn1_ber_encoder_encode_enumerated(UtObject *object, int64_t value);

/// Encode [value] as an EMBEDDED-PDV.
///
/// !arg-type value UtAsn1EmbeddedValue
size_t ut_asn1_ber_encoder_encode_embedded_pdv(UtObject *object,
                                               UtObject *value);

/// Encode [value] as a UTF8String.
///
/// !arg-type value UtString
size_t ut_asn1_ber_encoder_encode_utf8_string(UtObject *object,
                                              UtObject *value);

/// Encode [identifier] as a RELATIVE-OID
///
/// !arg-type identifier UtUint32List
size_t ut_asn1_ber_encoder_encode_relative_oid(UtObject *object,
                                               UtObject *identifier);

/// Encode [value] as a NumericString.
size_t ut_asn1_ber_encoder_encode_numeric_string(UtObject *object,
                                                 const char *value);

/// Encode [value] as a PrintableString.
size_t ut_asn1_ber_encoder_encode_printable_string(UtObject *object,
                                                   const char *value);

/// Encode [value] as an IA5String.
size_t ut_asn1_ber_encoder_encode_ia5_string(UtObject *object,
                                             const char *value);

/// Encode [value] as a UTCTime.
///
/// !arg-type value UtDateTime
size_t ut_asn1_ber_encoder_encode_utc_time(UtObject *object, UtObject *value);

/// Encode [value] as a GeneralizedTime.
///
/// !arg-type value UtDateTime
size_t ut_asn1_ber_encoder_encode_generalized_time(UtObject *object,
                                                   UtObject *value);

/// Encode [value] as a GraphicString.
size_t ut_asn1_ber_encoder_encode_graphic_string(UtObject *object,
                                                 const char *value);

/// Encode [value] as a VisibleString.
size_t ut_asn1_ber_encoder_encode_visible_string(UtObject *object,
                                                 const char *value);

/// Encode [value] as a GeneralString.
size_t ut_asn1_ber_encoder_encode_general_string(UtObject *object,
                                                 const char *value);

/// Encode [value] as a BMPString.
size_t ut_asn1_ber_encoder_encode_bmp_string(UtObject *object,
                                             const char *value);

/// Returns the encoded data.
/// Valid until something else is encoded.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_asn1_ber_encoder_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerEncoder].
bool ut_object_is_asn1_ber_encoder(UtObject *object);
