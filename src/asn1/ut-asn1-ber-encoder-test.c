#include <float.h>
#include <math.h>

#include "ut.h"

static void test_identifier() {
  UtObjectRef boolean_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      boolean_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  ut_assert_null_object(ut_asn1_encoder_get_error(boolean_encoder));
  UtObjectRef boolean_data = ut_asn1_ber_encoder_get_data(boolean_encoder);
  ut_assert_uint8_list_equal_hex(boolean_data, "01");

  UtObjectRef integer_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      integer_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_INTEGER);
  ut_assert_null_object(ut_asn1_encoder_get_error(integer_encoder));
  UtObjectRef integer_data = ut_asn1_ber_encoder_get_data(integer_encoder);
  ut_assert_uint8_list_equal_hex(integer_data, "02");

  UtObjectRef bit_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      bit_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BIT_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(bit_string_encoder));
  UtObjectRef bit_string_data =
      ut_asn1_ber_encoder_get_data(bit_string_encoder);
  ut_assert_uint8_list_equal_hex(bit_string_data, "03");

  UtObjectRef octet_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      octet_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OCTET_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(octet_string_encoder));
  UtObjectRef octet_string_data =
      ut_asn1_ber_encoder_get_data(octet_string_encoder);
  ut_assert_uint8_list_equal_hex(octet_string_data, "04");

  UtObjectRef null_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      null_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_NULL);
  ut_assert_null_object(ut_asn1_encoder_get_error(null_encoder));
  UtObjectRef null_data = ut_asn1_ber_encoder_get_data(null_encoder);
  ut_assert_uint8_list_equal_hex(null_data, "05");

  UtObjectRef object_identifier_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_identifier_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER);
  ut_assert_null_object(ut_asn1_encoder_get_error(object_identifier_encoder));
  UtObjectRef object_identifier_data =
      ut_asn1_ber_encoder_get_data(object_identifier_encoder);
  ut_assert_uint8_list_equal_hex(object_identifier_data, "06");

  UtObjectRef object_descriptor_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_descriptor_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR);
  ut_assert_null_object(ut_asn1_encoder_get_error(object_descriptor_encoder));
  UtObjectRef object_descriptor_data =
      ut_asn1_ber_encoder_get_data(object_descriptor_encoder);
  ut_assert_uint8_list_equal_hex(object_descriptor_data, "07");

  UtObjectRef external_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      external_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_EXTERNAL);
  ut_assert_null_object(ut_asn1_encoder_get_error(external_encoder));
  UtObjectRef external_data = ut_asn1_ber_encoder_get_data(external_encoder);
  ut_assert_uint8_list_equal_hex(external_data, "08");

  UtObjectRef real_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      real_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_REAL);
  ut_assert_null_object(ut_asn1_encoder_get_error(real_encoder));
  UtObjectRef real_data = ut_asn1_ber_encoder_get_data(real_encoder);
  ut_assert_uint8_list_equal_hex(real_data, "09");

  UtObjectRef enumerated_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      enumerated_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
  ut_assert_null_object(ut_asn1_encoder_get_error(enumerated_encoder));
  UtObjectRef enumerated_data =
      ut_asn1_ber_encoder_get_data(enumerated_encoder);
  ut_assert_uint8_list_equal_hex(enumerated_data, "0a");

  UtObjectRef utf8_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      utf8_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_UTF8_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(utf8_string_encoder));
  UtObjectRef utf8_string_data =
      ut_asn1_ber_encoder_get_data(utf8_string_encoder);
  ut_assert_uint8_list_equal_hex(utf8_string_data, "0c");

  UtObjectRef relative_oid_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      relative_oid_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID);
  ut_assert_null_object(ut_asn1_encoder_get_error(relative_oid_encoder));
  UtObjectRef relative_oid_data =
      ut_asn1_ber_encoder_get_data(relative_oid_encoder);
  ut_assert_uint8_list_equal_hex(relative_oid_data, "0d");

  UtObjectRef numeric_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      numeric_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(numeric_string_encoder));
  UtObjectRef numeric_string_data =
      ut_asn1_ber_encoder_get_data(numeric_string_encoder);
  ut_assert_uint8_list_equal_hex(numeric_string_data, "12");

  UtObjectRef printable_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      printable_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(printable_string_encoder));
  UtObjectRef printable_string_data =
      ut_asn1_ber_encoder_get_data(printable_string_encoder);
  ut_assert_uint8_list_equal_hex(printable_string_data, "13");

  UtObjectRef ia5_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      ia5_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_IA5_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(ia5_string_encoder));
  UtObjectRef ia5_string_data =
      ut_asn1_ber_encoder_get_data(ia5_string_encoder);
  ut_assert_uint8_list_equal_hex(ia5_string_data, "16");

  UtObjectRef visible_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      visible_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING);
  ut_assert_null_object(ut_asn1_encoder_get_error(visible_string_encoder));
  UtObjectRef visible_string_data =
      ut_asn1_ber_encoder_get_data(visible_string_encoder);
  ut_assert_uint8_list_equal_hex(visible_string_data, "1a");

  UtObjectRef sequence_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      sequence_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  ut_assert_null_object(ut_asn1_encoder_get_error(sequence_encoder));
  UtObjectRef sequence_data = ut_asn1_ber_encoder_get_data(sequence_encoder);
  ut_assert_uint8_list_equal_hex(sequence_data, "30");

  UtObjectRef set_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      set_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_SET);
  ut_assert_null_object(ut_asn1_encoder_get_error(set_encoder));
  UtObjectRef set_data = ut_asn1_ber_encoder_get_data(set_encoder);
  ut_assert_uint8_list_equal_hex(set_data, "31");
}

static void test_length() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder1, 0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder2, 1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder3, 127);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "7f");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder4, 128);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder4));
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "820080");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder5, 65535);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder5));
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "82ffff");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder6, 65536);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder6));
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "83010000");

  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder7, 16777215);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder7));
  UtObjectRef data7 = ut_asn1_ber_encoder_get_data(encoder7);
  ut_assert_uint8_list_equal_hex(data7, "83ffffff");

  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder8, 16777216);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder8));
  UtObjectRef data8 = ut_asn1_ber_encoder_get_data(encoder8);
  ut_assert_uint8_list_equal_hex(data8, "8401000000");

  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder9, 4294967295);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder9));
  UtObjectRef data9 = ut_asn1_ber_encoder_get_data(encoder9);
  ut_assert_uint8_list_equal_hex(data9, "84ffffffff");
}

static void test_boolean() {
  // False.
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder1, false);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  // True.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder2, true);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "ff");

  // Encoded as object with type.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef type3 = ut_asn1_boolean_type_new();
  UtObjectRef value3 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder3, type3, value3);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "0101ff");

  // Encoded with wrong value.
  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  UtObjectRef type4 = ut_asn1_boolean_type_new();
  UtObjectRef value4 = ut_int64_new(42);
  ut_asn1_encoder_encode_value(encoder4, type4, value4);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder4),
      "Unknown type UtInt64 provided for BOOLEAN");
}

static void test_integer() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder1, 0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder2, 1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder3, -1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "ff");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder4, 127);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder4));
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "7f");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder5, 128);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder5));
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "0080");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder6, -128);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder6));
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "80");

  // Maximum unsigned value
  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder7, INT64_MAX);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder7));
  UtObjectRef data7 = ut_asn1_ber_encoder_get_data(encoder7);
  ut_assert_uint8_list_equal_hex(data7, "7fffffffffffffff");

  // Minimum signed value
  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder8, INT64_MIN);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder8));
  UtObjectRef data8 = ut_asn1_ber_encoder_get_data(encoder8);
  ut_assert_uint8_list_equal_hex(data8, "8000000000000000");

  // 64 bit value.
  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder9, 0x0123456789abcdef);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder9));
  UtObjectRef data9 = ut_asn1_ber_encoder_get_data(encoder9);
  ut_assert_uint8_list_equal_hex(data9, "0123456789abcdef");

  // Encoded as object with type.
  UtObjectRef encoder10 = ut_asn1_ber_encoder_new();
  UtObjectRef type10 = ut_asn1_integer_type_new();
  UtObjectRef value10 = ut_int64_new(42);
  ut_asn1_encoder_encode_value(encoder10, type10, value10);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder10));
  UtObjectRef data10 = ut_asn1_ber_encoder_get_data(encoder10);
  ut_assert_uint8_list_equal_hex(data10, "02012a");

  // Encoded with wrong value.
  UtObjectRef encoder11 = ut_asn1_ber_encoder_new();
  UtObjectRef type11 = ut_asn1_integer_type_new();
  UtObjectRef value11 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder11, type11, value11);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder11),
      "Unknown type UtBoolean provided for INTEGER");
}

static void test_bit_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_bit_list_new_msb();
  ut_asn1_ber_encoder_encode_bit_string(encoder1, string1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_bit_list_new_msb_from_bin_string("00000000");
  ut_asn1_ber_encoder_encode_bit_string(encoder2, string2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "0000");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef string3 = ut_bit_list_new_msb_from_bin_string("1");
  ut_asn1_ber_encoder_encode_bit_string(encoder3, string3);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "0780");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  UtObjectRef string4 = ut_bit_list_new_msb_from_bin_string("1101");
  ut_asn1_ber_encoder_encode_bit_string(encoder4, string4);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder4));
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "04d0");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  UtObjectRef string5 =
      ut_bit_list_new_msb_from_bin_string("11111111000000001111000000001111");
  ut_asn1_ber_encoder_encode_bit_string(encoder5, string5);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder5));
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "00ff00f00f");

  // Encoded as object with type.
  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  UtObjectRef type6 = ut_asn1_bit_string_type_new();
  UtObjectRef value6 = ut_bit_list_new_msb_from_bin_string("1101");
  ut_asn1_encoder_encode_value(encoder6, type6, value6);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder6));
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "030204d0");

  // Encoded with wrong value.
  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  UtObjectRef type7 = ut_asn1_bit_string_type_new();
  UtObjectRef value7 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder7, type7, value7);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder7),
      "Unknown type UtBoolean provided for BIT STRING");
}

static void test_octet_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_uint8_list_new_from_hex_string("");
  ut_asn1_ber_encoder_encode_octet_string(encoder1, string1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_uint8_list_new_from_hex_string("00");
  ut_asn1_ber_encoder_encode_octet_string(encoder2, string2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "00");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef string3 = ut_uint8_list_new_from_hex_string("0123456789abcdef");
  ut_asn1_ber_encoder_encode_octet_string(encoder3, string3);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "0123456789abcdef");

  // Encoded as object with type.
  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  UtObjectRef type4 = ut_asn1_octet_string_type_new();
  UtObjectRef value4 = ut_uint8_list_new_from_hex_string("0123456789abcdef");
  ut_asn1_encoder_encode_value(encoder4, type4, value4);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder4));
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "04080123456789abcdef");

  // Encoded with wrong value.
  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  UtObjectRef type5 = ut_asn1_octet_string_type_new();
  UtObjectRef value5 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder5, type5, value5);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder5),
      "Unknown type UtBoolean provided for OCTET STRING");
}

static void test_null() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_null(encoder1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "");

  // Encoded as object with type.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef type2 = ut_asn1_null_type_new();
  UtObjectRef value2 = ut_null_new();
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "0500");

  // Encoded with wrong value.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef type3 = ut_asn1_null_type_new();
  UtObjectRef value3 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder3, type3, value3);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder3),
      "Unknown type UtBoolean provided for NULL");
}

static void test_object_identifier() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id1 = ut_uint32_list_new_from_elements(3, 2, 999, 3);
  ut_asn1_ber_encoder_encode_object_identifier(encoder1, id1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "883703");

  // Encoded as object with type.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef type2 = ut_asn1_object_identifier_type_new();
  UtObjectRef value2 = ut_uint32_list_new_from_elements(3, 2, 999, 3);
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "0603883703");

  // Encoded with wrong value.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef type3 = ut_asn1_object_identifier_type_new();
  UtObjectRef value3 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder3, type3, value3);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder3),
      "Unknown type UtBoolean provided for OBJECT IDENTIFIER");
}

static void test_real() {
  // Zero.
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder1, 0.0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "");

  // Integer.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder2, 10.0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "800105");

  // Negative number.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder3, -10.0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "c00105");

  // Pi.
  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder4, 3.14159);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder4));
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "80ce0c90fcf80dc337");

  // Largest double.
  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder5, DBL_MAX);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder5));
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "8103cb1fffffffffffff");

  // Smallest double.
  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder6, DBL_MIN);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder6));
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "81fc0201");

  // Infinity.
  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder7, INFINITY);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder7));
  UtObjectRef data7 = ut_asn1_ber_encoder_get_data(encoder7);
  ut_assert_uint8_list_equal_hex(data7, "40");

  // Minus infinity.
  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder8, -INFINITY);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder8));
  UtObjectRef data8 = ut_asn1_ber_encoder_get_data(encoder8);
  ut_assert_uint8_list_equal_hex(data8, "41");

  // Not a number.
  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder9, NAN);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder9));
  UtObjectRef data9 = ut_asn1_ber_encoder_get_data(encoder9);
  ut_assert_uint8_list_equal_hex(data9, "42");

  // Negative zero.
  UtObjectRef encoder10 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_real(encoder10, -0.0);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder10));
  UtObjectRef data10 = ut_asn1_ber_encoder_get_data(encoder10);
  ut_assert_uint8_list_equal_hex(data10, "43");

  // Encoded as object with type.
  UtObjectRef encoder11 = ut_asn1_ber_encoder_new();
  UtObjectRef type11 = ut_asn1_real_type_new();
  UtObjectRef value11 = ut_float64_new(10.0);
  ut_asn1_encoder_encode_value(encoder11, type11, value11);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder11));
  UtObjectRef data11 = ut_asn1_ber_encoder_get_data(encoder11);
  ut_assert_uint8_list_equal_hex(data11, "0903800105");

  // Encoded with wrong value.
  UtObjectRef encoder12 = ut_asn1_ber_encoder_new();
  UtObjectRef type12 = ut_asn1_real_type_new();
  UtObjectRef value12 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder12, type12, value12);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder12),
      "Unknown type UtBoolean provided for REAL");
}

static void test_enumerated() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_enumerated(encoder1, 42);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "2a");

  // Encoded as object with type.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef type2_items = ut_map_new();
  ut_map_insert_take(type2_items, ut_string_new("red"), ut_uint64_new(1));
  ut_map_insert_take(type2_items, ut_string_new("green"), ut_uint64_new(2));
  ut_map_insert_take(type2_items, ut_string_new("blue"), ut_uint64_new(3));
  UtObjectRef type2 = ut_asn1_enumerated_type_new(type2_items, false);
  UtObjectRef value2 = ut_string_new("green");
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "0a0102");
}

static void test_utf8_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello 😀");
  ut_asn1_ber_encoder_encode_utf8_string(encoder1, string1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20f09f9880");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_utf8_string(encoder2, string2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // Encoded as object with type.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef type3 = ut_asn1_utf8_string_type_new();
  UtObjectRef value3 = ut_string_new("Hello 😀");
  ut_asn1_encoder_encode_value(encoder3, type3, value3);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder3));
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "0c0a48656c6c6f20f09f9880");

  // FIXME: Invalid UTF8
}

static void test_relative_oid() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id1 = ut_uint32_list_new_from_elements(3, 8571, 3, 2);
  ut_asn1_ber_encoder_encode_relative_oid(encoder1, id1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "c27b0302");

  // Encoded as object with type.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef type2 = ut_asn1_relative_oid_type_new();
  UtObjectRef value2 = ut_uint32_list_new_from_elements(3, 8571, 3, 2);
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "0d04c27b0302");

  // Encoded with wrong value.
  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef type3 = ut_asn1_relative_oid_type_new();
  UtObjectRef value3 = ut_boolean_new(true);
  ut_asn1_encoder_encode_value(encoder3, type3, value3);
  ut_assert_is_error_with_description(
      ut_asn1_encoder_get_error(encoder3),
      "Unknown type UtBoolean provided for RELATIVE-OID");
}

static void test_sequence() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef components1 = ut_map_new_ordered();
  ut_map_insert_string_take(components1, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components1, "age", ut_asn1_integer_type_new());
  UtObjectRef type1 = ut_asn1_sequence_type_new(components1, false);
  UtObjectRef value1 = ut_map_new();
  ut_map_insert_string_take(value1, "name", ut_string_new("Arthur Dent"));
  ut_map_insert_string_take(value1, "age", ut_int64_new(42));
  ut_asn1_encoder_encode_value(encoder1, type1, value1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "30100c0b4172746875722044656e7402012a");

  // Empty sequence.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef components2 = ut_map_new_ordered();
  UtObjectRef type2 = ut_asn1_sequence_type_new(components2, false);
  UtObjectRef value2 = ut_map_new();
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "3000");

  // FIXME: Optional components
  // FIXME: Default components
  // FIXME: Tagged components
  // FIXME: Extensible
}

static void test_sequence_of() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef child_type1 = ut_asn1_integer_type_new();
  UtObjectRef type1 = ut_asn1_sequence_of_type_new(child_type1);
  UtObjectRef value1 = ut_list_new_from_elements_take(
      ut_int64_new(1), ut_int64_new(2), ut_int64_new(3), NULL);
  ut_asn1_encoder_encode_value(encoder1, type1, value1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "3009020101020102020103");

  // Empty list.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef child_type2 = ut_asn1_integer_type_new();
  UtObjectRef type2 = ut_asn1_sequence_of_type_new(child_type2);
  UtObjectRef value2 = ut_list_new();
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "3000");
}

static void test_set() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef components1 = ut_map_new_ordered();
  ut_map_insert_string_take(components1, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components1, "age", ut_asn1_integer_type_new());
  UtObjectRef type1 = ut_asn1_set_type_new(components1, false);
  UtObjectRef value1 = ut_map_new();
  ut_map_insert_string_take(value1, "name", ut_string_new("Arthur Dent"));
  ut_map_insert_string_take(value1, "age", ut_int64_new(42));
  ut_asn1_encoder_encode_value(encoder1, type1, value1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "31100c0b4172746875722044656e7402012a");

  // Empty set.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef components2 = ut_map_new_ordered();
  UtObjectRef type2 = ut_asn1_set_type_new(components2, false);
  UtObjectRef value2 = ut_map_new();
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "3100");

  // FIXME: Optional components
  // FIXME: Default components
  // FIXME: Tagged components
  // FIXME: Extensible
}

static void test_set_of() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef child_type1 = ut_asn1_integer_type_new();
  UtObjectRef type1 = ut_asn1_set_of_type_new(child_type1);
  UtObjectRef value1 = ut_list_new_from_elements_take(
      ut_int64_new(1), ut_int64_new(2), ut_int64_new(3), NULL);
  ut_asn1_encoder_encode_value(encoder1, type1, value1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "3109020101020102020103");

  // Empty list.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef child_type2 = ut_asn1_integer_type_new();
  UtObjectRef type2 = ut_asn1_set_of_type_new(child_type2);
  UtObjectRef value2 = ut_list_new();
  ut_asn1_encoder_encode_value(encoder2, type2, value2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "3100");
}

static void test_numeric_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder1, "12345 67890");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "3132333435203637383930");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder2, "");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Encoded as object with type.

  // FIXME: Invalid characters
}

static void test_printable_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder1, "Hello World");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder2, "");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Encoded as object with type.

  // FIXME: Invalid characters
}

static void test_ia5_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello World");
  ut_asn1_ber_encoder_encode_ia5_string(encoder1, string1);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_ia5_string(encoder2, string2);
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Encoded as object with type.

  // FIXME: Invalid characters
}

static void test_visible_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder1, "Hello World");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder1));
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder2, "");
  ut_assert_null_object(ut_asn1_encoder_get_error(encoder2));
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Encoded as object with type.

  // FIXME: Invalid characters
}

static void test_choice() {
  // FIXME
}

static void test_tagged_type() {
  // FIXME
}

int main(int argc, char **argv) {
  test_identifier();
  test_length();
  test_boolean();
  test_integer();
  test_bit_string();
  test_octet_string();
  test_null();
  test_object_identifier();
  test_real();
  test_enumerated();
  test_utf8_string();
  test_relative_oid();
  test_sequence();
  test_sequence_of();
  test_set();
  test_set_of();
  test_numeric_string();
  test_printable_string();
  test_ia5_string();
  test_visible_string();
  test_choice();
  test_tagged_type();

  return 0;
}
