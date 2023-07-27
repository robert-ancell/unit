#include "ut.h"

static void test_identifier() {
  UtObjectRef boolean_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      boolean_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  UtObjectRef boolean_data = ut_asn1_ber_encoder_get_data(boolean_encoder);
  ut_assert_uint8_list_equal_hex(boolean_data, "01");

  UtObjectRef integer_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      integer_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_INTEGER);
  UtObjectRef integer_data = ut_asn1_ber_encoder_get_data(integer_encoder);
  ut_assert_uint8_list_equal_hex(integer_data, "02");

  UtObjectRef bit_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      bit_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BIT_STRING);
  UtObjectRef bit_string_data =
      ut_asn1_ber_encoder_get_data(bit_string_encoder);
  ut_assert_uint8_list_equal_hex(bit_string_data, "03");

  UtObjectRef octet_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      octet_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OCTET_STRING);
  UtObjectRef octet_string_data =
      ut_asn1_ber_encoder_get_data(octet_string_encoder);
  ut_assert_uint8_list_equal_hex(octet_string_data, "04");

  UtObjectRef null_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      null_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_NULL);
  UtObjectRef null_data = ut_asn1_ber_encoder_get_data(null_encoder);
  ut_assert_uint8_list_equal_hex(null_data, "05");

  UtObjectRef object_identifier_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_identifier_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER);
  UtObjectRef object_identifier_data =
      ut_asn1_ber_encoder_get_data(object_identifier_encoder);
  ut_assert_uint8_list_equal_hex(object_identifier_data, "06");

  UtObjectRef object_descriptor_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_descriptor_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR);
  UtObjectRef object_descriptor_data =
      ut_asn1_ber_encoder_get_data(object_descriptor_encoder);
  ut_assert_uint8_list_equal_hex(object_descriptor_data, "07");

  UtObjectRef external_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      external_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_EXTERNAL);
  UtObjectRef external_data = ut_asn1_ber_encoder_get_data(external_encoder);
  ut_assert_uint8_list_equal_hex(external_data, "08");

  UtObjectRef real_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      real_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_REAL);
  UtObjectRef real_data = ut_asn1_ber_encoder_get_data(real_encoder);
  ut_assert_uint8_list_equal_hex(real_data, "09");

  UtObjectRef enumerated_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      enumerated_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
  UtObjectRef enumerated_data =
      ut_asn1_ber_encoder_get_data(enumerated_encoder);
  ut_assert_uint8_list_equal_hex(enumerated_data, "0a");

  UtObjectRef utf8_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      utf8_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_UTF8_STRING);
  UtObjectRef utf8_string_data =
      ut_asn1_ber_encoder_get_data(utf8_string_encoder);
  ut_assert_uint8_list_equal_hex(utf8_string_data, "0c");

  UtObjectRef relative_object_identifier_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      relative_object_identifier_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_RELATIVE_OBJECT_IDENTIFIER);
  UtObjectRef relative_object_identifier_data =
      ut_asn1_ber_encoder_get_data(relative_object_identifier_encoder);
  ut_assert_uint8_list_equal_hex(relative_object_identifier_data, "0d");

  UtObjectRef numeric_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      numeric_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING);
  UtObjectRef numeric_string_data =
      ut_asn1_ber_encoder_get_data(numeric_string_encoder);
  ut_assert_uint8_list_equal_hex(numeric_string_data, "12");

  UtObjectRef printable_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      printable_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING);
  UtObjectRef printable_string_data =
      ut_asn1_ber_encoder_get_data(printable_string_encoder);
  ut_assert_uint8_list_equal_hex(printable_string_data, "13");

  UtObjectRef ia5_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      ia5_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_IA5_STRING);
  UtObjectRef ia5_string_data =
      ut_asn1_ber_encoder_get_data(ia5_string_encoder);
  ut_assert_uint8_list_equal_hex(ia5_string_data, "16");

  UtObjectRef visible_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      visible_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING);
  UtObjectRef visible_string_data =
      ut_asn1_ber_encoder_get_data(visible_string_encoder);
  ut_assert_uint8_list_equal_hex(visible_string_data, "1a");

  UtObjectRef sequence_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      sequence_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  UtObjectRef sequence_data = ut_asn1_ber_encoder_get_data(sequence_encoder);
  ut_assert_uint8_list_equal_hex(sequence_data, "30");

  UtObjectRef set_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      set_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_SET);
  UtObjectRef set_data = ut_asn1_ber_encoder_get_data(set_encoder);
  ut_assert_uint8_list_equal_hex(set_data, "31");
}

static void test_length() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder1, 0);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder2, 1);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder3, 127);
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "7f");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder4, 128);
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "820080");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder5, 65535);
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "82ffff");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder6, 65536);
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "83010000");

  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder7, 16777215);
  UtObjectRef data7 = ut_asn1_ber_encoder_get_data(encoder7);
  ut_assert_uint8_list_equal_hex(data7, "83ffffff");

  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder8, 16777216);
  UtObjectRef data8 = ut_asn1_ber_encoder_get_data(encoder8);
  ut_assert_uint8_list_equal_hex(data8, "8401000000");

  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder9, 4294967295);
  UtObjectRef data9 = ut_asn1_ber_encoder_get_data(encoder9);
  ut_assert_uint8_list_equal_hex(data9, "84ffffffff");
}

static void test_boolean() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder1, false);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder2, true);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "ff");
}

static void test_integer() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder1, 0);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder2, 1);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder3, -1);
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "ff");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder4, 127);
  UtObjectRef data4 = ut_asn1_ber_encoder_get_data(encoder4);
  ut_assert_uint8_list_equal_hex(data4, "7f");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder5, 128);
  UtObjectRef data5 = ut_asn1_ber_encoder_get_data(encoder5);
  ut_assert_uint8_list_equal_hex(data5, "0080");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder6, -128);
  UtObjectRef data6 = ut_asn1_ber_encoder_get_data(encoder6);
  ut_assert_uint8_list_equal_hex(data6, "80");

  // Maximum unsigned value
  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder7, INT64_MAX);
  UtObjectRef data7 = ut_asn1_ber_encoder_get_data(encoder7);
  ut_assert_uint8_list_equal_hex(data7, "7fffffffffffffff");

  // Minimum signed value
  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder8, INT64_MIN);
  UtObjectRef data8 = ut_asn1_ber_encoder_get_data(encoder8);
  ut_assert_uint8_list_equal_hex(data8, "8000000000000000");

  // 64 bit value.
  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder9, 0x0123456789abcdef);
  UtObjectRef data9 = ut_asn1_ber_encoder_get_data(encoder9);
  ut_assert_uint8_list_equal_hex(data9, "0123456789abcdef");
}

static void test_octet_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_uint8_list_new_from_hex_string("");
  ut_asn1_ber_encoder_encode_octet_string(encoder1, string1);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_uint8_list_new_from_hex_string("00");
  ut_asn1_ber_encoder_encode_octet_string(encoder2, string2);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "00");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef string3 = ut_uint8_list_new_from_hex_string("0123456789abcdef");
  ut_asn1_ber_encoder_encode_octet_string(encoder3, string3);
  UtObjectRef data3 = ut_asn1_ber_encoder_get_data(encoder3);
  ut_assert_uint8_list_equal_hex(data3, "0123456789abcdef");
}

static void test_null() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_null(encoder1);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "");
}

static void test_object_identifier() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id = ut_uint32_list_new_from_elements(3, 2, 999, 3);
  ut_asn1_ber_encoder_encode_object_identifier(encoder1, id);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "883703");
}

static void test_enumerated() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_enumerated(encoder1, 42);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "2a");
}

static void test_utf8_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello 😀");
  ut_asn1_ber_encoder_encode_utf8_string(encoder1, string1);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20f09f9880");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_utf8_string(encoder2, string2);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Invalid UTF8
}

static void test_relative_object_identifier() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id = ut_uint32_list_new_from_elements(3, 8571, 3, 2);
  ut_asn1_ber_encoder_encode_relative_object_identifier(encoder1, id);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "c27b0302");
}

static void test_numeric_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder1, "12345 67890");
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "3132333435203637383930");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder2, "");
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Invalid characters
}

static void test_printable_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder1, "Hello World");
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder2, "");
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Invalid characters
}

static void test_ia5_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello World");
  ut_asn1_ber_encoder_encode_ia5_string(encoder1, string1);
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_ia5_string(encoder2, string2);
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Invalid characters
}

static void test_visible_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder1, "Hello World");
  UtObjectRef data1 = ut_asn1_ber_encoder_get_data(encoder1);
  ut_assert_uint8_list_equal_hex(data1, "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder2, "");
  UtObjectRef data2 = ut_asn1_ber_encoder_get_data(encoder2);
  ut_assert_uint8_list_equal_hex(data2, "");

  // FIXME: Invalid characters
}

static void test_sequence() {
  UtObjectRef encoder = ut_asn1_ber_encoder_new();
  size_t length = ut_asn1_ber_encoder_encode_integer(encoder, 42);
  length += ut_asn1_ber_encoder_encode_definite_length(encoder, 1);
  length += ut_asn1_ber_encoder_encode_primitive_identifier(
      encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_INTEGER);
  length += ut_asn1_ber_encoder_encode_boolean(encoder, true);
  length += ut_asn1_ber_encoder_encode_definite_length(encoder, 1);
  length += ut_asn1_ber_encoder_encode_primitive_identifier(
      encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  ut_asn1_ber_encoder_encode_definite_length(encoder, length);
  ut_asn1_ber_encoder_encode_constructed_identifier(
      encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  UtObjectRef data = ut_asn1_ber_encoder_get_data(encoder);
  ut_assert_uint8_list_equal_hex(data, "30060101ff02012a");
}

int main(int argc, char **argv) {
  test_identifier();
  test_length();
  test_boolean();
  test_integer();
  test_octet_string();
  test_null();
  test_object_identifier();
  test_enumerated();
  test_utf8_string();
  test_relative_object_identifier();
  test_numeric_string();
  test_printable_string();
  test_ia5_string();
  test_visible_string();

  test_sequence();

  return 0;
}
