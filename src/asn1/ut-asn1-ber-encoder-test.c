#include "ut.h"

static void test_identifier() {
  UtObjectRef boolean_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      boolean_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(boolean_encoder),
                                 "01");

  UtObjectRef integer_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      integer_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_INTEGER);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(integer_encoder),
                                 "02");

  UtObjectRef bit_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      bit_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_BIT_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(bit_string_encoder), "03");

  UtObjectRef octet_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      octet_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OCTET_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(octet_string_encoder), "04");

  UtObjectRef null_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      null_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_NULL);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(null_encoder),
                                 "05");

  UtObjectRef object_identifier_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_identifier_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(object_identifier_encoder), "06");

  UtObjectRef object_descriptor_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      object_descriptor_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(object_descriptor_encoder), "07");

  UtObjectRef external_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      external_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_EXTERNAL);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(external_encoder),
                                 "08");

  UtObjectRef real_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      real_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_REAL);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(real_encoder),
                                 "09");

  UtObjectRef enumerated_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      enumerated_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(enumerated_encoder), "0a");

  UtObjectRef utf8_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      utf8_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_UTF8_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(utf8_string_encoder), "0c");

  UtObjectRef relative_object_identifier_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      relative_object_identifier_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_RELATIVE_OBJECT_IDENTIFIER);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(relative_object_identifier_encoder), "0d");

  UtObjectRef numeric_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      numeric_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(numeric_string_encoder), "12");

  UtObjectRef printable_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      printable_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(printable_string_encoder), "13");

  UtObjectRef ia5_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      ia5_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_IA5_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(ia5_string_encoder), "16");

  UtObjectRef visible_string_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_primitive_identifier(
      visible_string_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING);
  ut_assert_uint8_list_equal_hex(
      ut_asn1_ber_encoder_get_data(visible_string_encoder), "1a");

  UtObjectRef sequence_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      sequence_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL,
      UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(sequence_encoder),
                                 "30");

  UtObjectRef set_encoder = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_constructed_identifier(
      set_encoder, UT_ASN1_TAG_CLASS_UNIVERSAL, UT_ASN1_TAG_UNIVERSAL_SET);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(set_encoder),
                                 "31");
}

static void test_length() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder1, 0);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder2, 1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder3, 127);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder3), "7f");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder4, 128);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder4),
                                 "820080");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder5, 65535);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder5),
                                 "82ffff");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder6, 65536);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder6),
                                 "83010000");

  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder7, 16777215);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder7),
                                 "83ffffff");

  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder8, 16777216);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder8),
                                 "8401000000");

  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_definite_length(encoder9, 4294967295);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder9),
                                 "84ffffffff");
}

static void test_boolean() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder1, false);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_boolean(encoder2, true);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "ff");
}

static void test_integer() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder1, 0);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "00");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder2, 1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "01");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder3, -1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder3), "ff");

  UtObjectRef encoder4 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder4, 127);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder4), "7f");

  UtObjectRef encoder5 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder5, 128);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder5),
                                 "0080");

  UtObjectRef encoder6 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder6, -128);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder6), "80");

  // Maximum unsigned value
  UtObjectRef encoder7 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder7, INT64_MAX);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder7),
                                 "7fffffffffffffff");

  // Minimum signed value
  UtObjectRef encoder8 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder8, INT64_MIN);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder8),
                                 "8000000000000000");

  // 64 bit value.
  UtObjectRef encoder9 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_integer(encoder9, 0x0123456789abcdef);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder9),
                                 "0123456789abcdef");
}

static void test_octet_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_uint8_list_new_from_hex_string("");
  ut_asn1_ber_encoder_encode_octet_string(encoder1, string1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "");

  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_uint8_list_new_from_hex_string("00");
  ut_asn1_ber_encoder_encode_octet_string(encoder2, string2);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "00");

  UtObjectRef encoder3 = ut_asn1_ber_encoder_new();
  UtObjectRef string3 = ut_uint8_list_new_from_hex_string("0123456789abcdef");
  ut_asn1_ber_encoder_encode_octet_string(encoder3, string3);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder3),
                                 "0123456789abcdef");
}

static void test_null() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_null(encoder1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "");
}

static void test_object_identifier() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id = ut_uint32_list_new_from_elements(3, 2, 999, 3);
  ut_asn1_ber_encoder_encode_object_identifier(encoder1, id);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "883703");
}

static void test_enumerated() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_enumerated(encoder1, 42);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1), "2a");
}

static void test_utf8_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello 😀");
  ut_asn1_ber_encoder_encode_utf8_string(encoder1, string1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "48656c6c6f20f09f9880");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_utf8_string(encoder2, string2);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "");

  // FIXME: Invalid UTF8
}

static void test_relative_object_identifier() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef id = ut_uint32_list_new_from_elements(3, 8571, 3, 2);
  ut_asn1_ber_encoder_encode_relative_object_identifier(encoder1, id);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "c27b0302");
}

static void test_numeric_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder1, "12345 67890");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "3132333435203637383930");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_numeric_string(encoder2, "");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "");

  // FIXME: Invalid characters
}

static void test_printable_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder1, "Hello World");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_printable_string(encoder2, "");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "");

  // FIXME: Invalid characters
}

static void test_ia5_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  UtObjectRef string1 = ut_string_new("Hello World");
  ut_asn1_ber_encoder_encode_ia5_string(encoder1, string1);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  UtObjectRef string2 = ut_string_new("");
  ut_asn1_ber_encoder_encode_ia5_string(encoder2, string2);
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "");

  // FIXME: Invalid characters
}

static void test_visible_string() {
  UtObjectRef encoder1 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder1, "Hello World");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder1),
                                 "48656c6c6f20576f726c64");

  // Empty string.
  UtObjectRef encoder2 = ut_asn1_ber_encoder_new();
  ut_asn1_ber_encoder_encode_visible_string(encoder2, "");
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder2), "");

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
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_encoder_get_data(encoder),
                                 "30060101ff02012a");
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
