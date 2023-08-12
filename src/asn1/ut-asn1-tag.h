#pragma once

#include "ut-object.h"

typedef enum {
  UT_ASN1_TAG_CLASS_UNIVERSAL,
  UT_ASN1_TAG_CLASS_APPLICATION,
  UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC,
  UT_ASN1_TAG_CLASS_PRIVATE
} UtAsn1TagClass;

typedef enum {
  UT_ASN1_TAG_UNIVERSAL_BOOLEAN = 1,
  UT_ASN1_TAG_UNIVERSAL_INTEGER = 2,
  UT_ASN1_TAG_UNIVERSAL_BIT_STRING = 3,
  UT_ASN1_TAG_UNIVERSAL_OCTET_STRING = 4,
  UT_ASN1_TAG_UNIVERSAL_NULL = 5,
  UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER = 6,
  UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR = 7,
  UT_ASN1_TAG_UNIVERSAL_EXTERNAL = 8,
  UT_ASN1_TAG_UNIVERSAL_REAL = 9,
  UT_ASN1_TAG_UNIVERSAL_ENUMERATED = 10,
  UT_ASN1_TAG_UNIVERSAL_EMBEDDED_PDV = 11,
  UT_ASN1_TAG_UNIVERSAL_UTF8_STRING = 12,
  UT_ASN1_TAG_UNIVERSAL_RELATIVE_OBJECT_IDENTIFIER = 13,
  UT_ASN1_TAG_UNIVERSAL_SEQUENCE = 16,
  UT_ASN1_TAG_UNIVERSAL_SET = 17,
  UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING = 18,
  UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING = 19,
  UT_ASN1_TAG_UNIVERSAL_TELETEX_STRING = 20,
  UT_ASN1_TAG_UNIVERSAL_VIDEOTEX_STRING = 21,
  UT_ASN1_TAG_UNIVERSAL_IA5_STRING = 22,
  UT_ASN1_TAG_UNIVERSAL_UTC_TIME = 23,
  UT_ASN1_TAG_UNIVERSAL_GENERALIZED_TIME = 24,
  UT_ASN1_TAG_UNIVERSAL_GRAPHIC_STRING = 25,
  UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING = 26,
  UT_ASN1_TAG_UNIVERSAL_GENERAL_STRING = 27,
  UT_ASN1_TAG_UNIVERSAL_UNIVERSAL_STRING = 28,
  UT_ASN1_TAG_UNIVERSAL_CHARACTER_STRING = 29,
  UT_ASN1_TAG_UNIVERSAL_BMP_STRING = 30,
} UtAsn1TagUniversal;

/// Creates a new [UtAsn1Tag].
///
/// !return-ref
UtObject *ut_asn1_tag_new(UtAsn1TagClass class, uint32_t number);

UtAsn1TagClass ut_asn1_tag_get_class(UtObject *object);

uint32_t ut_asn1_tag_get_number(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1Tag].
bool ut_object_is_asn1_tag(UtObject *object);
