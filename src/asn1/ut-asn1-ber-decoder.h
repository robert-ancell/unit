#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates an ASN1. BER decoder to decode [data].
///
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtAsn1BerDecoder
UtObject *ut_asn1_ber_decoder_new(UtObject *data);

/// Returns the tag for the BER value.
///
/// !return-type UtAsn1Tag
UtObject *ut_asn1_ber_decoder_get_tag(UtObject *object);

/// Returns true if this BER value has constructed contents.
bool ut_asn1_ber_decoder_get_constructed(UtObject *object);

/// Returns the number of bytes used in this BER value.
size_t ut_asn1_ber_decoder_get_length(UtObject *object);

/// Returns the contents of this BER value.
///
/// !return-type UtUint8List
UtObject *ut_asn1_ber_decoder_get_contents(UtObject *object);

/// Returns the contents as a BOOLEAN value.
bool ut_asn1_ber_decoder_decode_boolean(UtObject *object);

/// Returns the contents as an INTEGER value.
int64_t ut_asn1_ber_decoder_decode_integer(UtObject *object);

/// Returns the contents as a BIT STRING value.
///
/// !return-ref
/// !return-type UtBitList.
UtObject *ut_asn1_ber_decoder_decode_bit_string(UtObject *object);

/// Returns the contents as an OCTET STRING value.
///
/// !return-ref
/// !return-type UtUint8List.
UtObject *ut_asn1_ber_decoder_decode_octet_string(UtObject *object);

/// Checks the contents as a NULL value.
void ut_asn1_ber_decoder_decode_null(UtObject *object);

/// Returns the contents as an OBJECT IDENTIFIER value.
///
/// !return-ref
/// !return-type UtObjectIdentifier.
UtObject *ut_asn1_ber_decoder_decode_object_identifier(UtObject *object);

/// Returns the contents as an ObjectDescriptor value.
char *ut_asn1_ber_decoder_decode_object_descriptor(UtObject *object);

/// Returns the contents as an EXTERNAL value.
///
/// !return-ref
/// !return-type UtAsn1EmbeddedValue.
UtObject *ut_asn1_ber_decoder_decode_external(UtObject *object);

/// Returns the contents as a REAL value.
double ut_asn1_ber_decoder_decode_real(UtObject *object);

/// Returns the contents as an ENUMERATED value.
int64_t ut_asn1_ber_decoder_decode_enumerated(UtObject *object);

/// Returns the contents as an EMBEDDED-PDV value.
///
/// !return-ref
/// !return-type UtAsn1EmbeddedValue.
UtObject *ut_asn1_ber_decoder_decode_embedded_pdv(UtObject *object);

/// Returns the contents as a UTF8String value.
char *ut_asn1_ber_decoder_decode_utf8_string(UtObject *object);

/// Returns the contents as an RELATIVE-OID value.
///
/// !return-ref
/// !return-type UtUint32List.
UtObject *ut_asn1_ber_decoder_decode_relative_oid(UtObject *object);

/// Returns the contents as a SEQUENCE value.
///
/// !return-ref
/// !return-type UtMap.
UtObject *ut_asn1_ber_decoder_decode_sequence(UtObject *object);

/// Returns the contents as a SET value.
///
/// !return-ref
/// !return-type UtMap.
UtObject *ut_asn1_ber_decoder_decode_set(UtObject *object);

/// Returns the contents as a NumericString value.
char *ut_asn1_ber_decoder_decode_numeric_string(UtObject *object);

/// Returns the contents as a PrintableString value.
char *ut_asn1_ber_decoder_decode_printable_string(UtObject *object);

/// Returns the contents as a IA5String value.
char *ut_asn1_ber_decoder_decode_ia5_string(UtObject *object);

/// Returns the contents as a UTCTime value.
///
/// !return-ref
/// !return-type UtDateTime.
UtObject *ut_asn1_ber_decoder_decode_utc_time(UtObject *object);

/// Returns the contents as a GeneralizedTime value.
///
/// !return-ref
/// !return-type UtDateTime.
UtObject *ut_asn1_ber_decoder_decode_generalized_time(UtObject *object);

/// Returns the contents as a GraphicString value.
char *ut_asn1_ber_decoder_decode_graphic_string(UtObject *object);

/// Returns the contents as a VisibleString value.
char *ut_asn1_ber_decoder_decode_visible_string(UtObject *object);

/// Returns the contents as a GeneralString value.
char *ut_asn1_ber_decoder_decode_general_string(UtObject *object);

/// Returns the contents as a BMPString value.
char *ut_asn1_ber_decoder_decode_bmp_string(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1BerDecoder].
bool ut_object_is_asn1_ber_decoder(UtObject *object);
