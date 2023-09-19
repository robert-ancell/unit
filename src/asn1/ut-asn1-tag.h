#pragma once

#include "ut-object.h"

/// ASN.1 tag type:
/// - [UT_ASN1_TAG_CLASS_UNIVERSAL] - a standard tag as defined in
/// [UtAsn1TagUniversal].
/// - [UT_ASN1_TAG_CLASS_APPLICATION] - a tag scoped to this application.
/// - [UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC] - a tag scoped to this element.
/// - [UT_ASN1_TAG_CLASS_PRIVATE] - a private tag.
typedef enum {
  UT_ASN1_TAG_CLASS_UNIVERSAL,
  UT_ASN1_TAG_CLASS_APPLICATION,
  UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC,
  UT_ASN1_TAG_CLASS_PRIVATE
} UtAsn1TagClass;

/// ASN.1 universal tag numbers.
/// - [UT_ASN1_TAG_UNIVERSAL_BOOLEAN] - tag for BOOLEAN type.
/// - [UT_ASN1_TAG_UNIVERSAL_INTEGER] - tag for INTEGER type.
/// - [UT_ASN1_TAG_UNIVERSAL_BIT_STRING] - tag for BIT STRING type.
/// - [UT_ASN1_TAG_UNIVERSAL_OCTET_STRING] - tag for OCTET STRING type.
/// - [UT_ASN1_TAG_UNIVERSAL_NULL] - tag for NULL type.
/// - [UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER] - tag for OBJECT IDENTIFIER
/// type.
/// - [UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR] - tag for ObjectDescriptor type.
/// - [UT_ASN1_TAG_UNIVERSAL_EXTERNAL] - tag for EXTERNAL type.
/// - [UT_ASN1_TAG_UNIVERSAL_REAL] - tag for REAL type.
/// - [UT_ASN1_TAG_UNIVERSAL_ENUMERATED] - tag for ENUMERATED type.
/// - [UT_ASN1_TAG_UNIVERSAL_EMBEDDED_PDV] - tag for EMBEDDED PDV type.
/// - [UT_ASN1_TAG_UNIVERSAL_UTF8_STRING] - tag for UTF8String type.
/// - [UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID] - tag for RELATIVE-OID type.
/// - [UT_ASN1_TAG_UNIVERSAL_SEQUENCE] - tag for SEQUENCE type.
/// - [UT_ASN1_TAG_UNIVERSAL_SET] - tag for SET type.
/// - [UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING] - tag for NumericString type.
/// - [UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING] - tag for Printabletring type.
/// - [UT_ASN1_TAG_UNIVERSAL_TELETEX_STRING] - tag for TeletexString type.
/// - [UT_ASN1_TAG_UNIVERSAL_VIDEOTEX_STRING] - tag for VideotexString type.
/// - [UT_ASN1_TAG_UNIVERSAL_IA5_STRING] - tag for IA5String type.
/// - [UT_ASN1_TAG_UNIVERSAL_UTC_TIME] - tag for UTCTime type.
/// - [UT_ASN1_TAG_UNIVERSAL_GENERALIZED_TIME] - tag for GeneralizedTime type.
/// - [UT_ASN1_TAG_UNIVERSAL_GRAPHIC_STRING] - tag for GraphicsString type.
/// - [UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING] - tag for VisibleString type.
/// - [UT_ASN1_TAG_UNIVERSAL_GENERAL_STRING] - tag for GeneralString type.
/// - [UT_ASN1_TAG_UNIVERSAL_UNIVERSAL_STRING] - tag for UniversalString type.
/// - [UT_ASN1_TAG_UNIVERSAL_CHARACTER_STRING] - tag for CHARACTER STRING type.
/// - [UT_ASN1_TAG_UNIVERSAL_BMP_STRING] - tag for BMPString type.
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
  UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID = 13,
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
  UT_ASN1_TAG_UNIVERSAL_BMP_STRING = 30
} UtAsn1TagUniversal;

/// Creates a new ASN.1 tag with [class] and [number].
///
/// !return-ref
/// !return-type UtAsn1Tag
UtObject *ut_asn1_tag_new(UtAsn1TagClass class, uint32_t number);

/// Creates a new ASN.1 tag with universal [type].
///
/// !return-ref
/// !return-type UtAsn1Tag
UtObject *ut_asn1_tag_new_universal(UtAsn1TagUniversal type);

/// Returns the class of this tag.
UtAsn1TagClass ut_asn1_tag_get_class(UtObject *object);

/// Returns the number of this tag.
uint32_t ut_asn1_tag_get_number(UtObject *object);

/// Returns true if this tag matches [class] and [number].
bool ut_asn1_tag_matches(UtObject *object, UtAsn1TagClass class,
                         uint32_t number);

/// Returns this tag formatted in ASN.1 form, e.g. "[UNIVERSAL 12]".
char *ut_asn1_tag_to_string(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1Tag].
bool ut_object_is_asn1_tag(UtObject *object);
