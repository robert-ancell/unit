#include <assert.h>

#include "ut.h"

static void test_boolean() {
  // False is zero.
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("010100");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_BOOLEAN);
  ut_assert_false(ut_asn1_ber_decoder_decode_boolean(decoder1));
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Any non-zero value is true.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0101ff");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_assert_true(ut_asn1_ber_decoder_decode_boolean(decoder2));
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("010101");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_assert_true(ut_asn1_ber_decoder_decode_boolean(decoder3));
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));

  // Decoded as object with type.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("0101ff");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_boolean_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_is_boolean(value4));
  ut_assert_true(ut_boolean_get_value(value4));

  // Empty data.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("0100");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_asn1_ber_decoder_decode_boolean(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid BOOLEAN data length");

  // Too much data.
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("01020000");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  ut_asn1_ber_decoder_decode_boolean(decoder6);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder6),
                                      "Invalid BOOLEAN data length");

  // Constructed form.
  UtObjectRef data7 = ut_uint8_list_new_from_hex_string("210100");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  ut_asn1_ber_decoder_decode_boolean(decoder7);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder7),
                                      "BOOLEAN does not have constructed form");
}

static void test_integer() {
  UtObjectRef data1a = ut_uint8_list_new_from_hex_string("020100");
  UtObjectRef decoder1a = ut_asn1_ber_decoder_new(data1a);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1a),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1a),
                      UT_ASN1_TAG_UNIVERSAL_INTEGER);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1a), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1a));

  // 0 in various lengths
  UtObjectRef data1b = ut_uint8_list_new_from_hex_string("02020000");
  UtObjectRef decoder1b = ut_asn1_ber_decoder_new(data1b);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1b), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1b));
  UtObjectRef data1c = ut_uint8_list_new_from_hex_string("0203000000");
  UtObjectRef decoder1c = ut_asn1_ber_decoder_new(data1c);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1c), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1c));
  UtObjectRef data1d = ut_uint8_list_new_from_hex_string("020400000000");
  UtObjectRef decoder1d = ut_asn1_ber_decoder_new(data1d);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1d), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1d));
  UtObjectRef data1e = ut_uint8_list_new_from_hex_string("02050000000000");
  UtObjectRef decoder1e = ut_asn1_ber_decoder_new(data1e);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1e), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1e));
  UtObjectRef data1f = ut_uint8_list_new_from_hex_string("0206000000000000");
  UtObjectRef decoder1f = ut_asn1_ber_decoder_new(data1f);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1f), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1f));
  UtObjectRef data1g = ut_uint8_list_new_from_hex_string("020700000000000000");
  UtObjectRef decoder1g = ut_asn1_ber_decoder_new(data1g);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1g), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1g));
  UtObjectRef data1h =
      ut_uint8_list_new_from_hex_string("02080000000000000000");
  UtObjectRef decoder1h = ut_asn1_ber_decoder_new(data1h);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder1h), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1h));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("020101");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder2), 1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("02017f");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder3), 127);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));

  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("02020080");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder4), 128);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));

  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("020180");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder5), -128);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder5));

  // -1 in various lengths
  UtObjectRef data6a = ut_uint8_list_new_from_hex_string("0201ff");
  UtObjectRef decoder6a = ut_asn1_ber_decoder_new(data6a);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6a), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6a));
  UtObjectRef data6b = ut_uint8_list_new_from_hex_string("0202ffff");
  UtObjectRef decoder6b = ut_asn1_ber_decoder_new(data6b);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6b), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6b));
  UtObjectRef data6c = ut_uint8_list_new_from_hex_string("0203ffffff");
  UtObjectRef decoder6c = ut_asn1_ber_decoder_new(data6c);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6c), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6c));
  UtObjectRef data6d = ut_uint8_list_new_from_hex_string("0204ffffffff");
  UtObjectRef decoder6d = ut_asn1_ber_decoder_new(data6d);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6d), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6d));
  UtObjectRef data6e = ut_uint8_list_new_from_hex_string("0205ffffffffff");
  UtObjectRef decoder6e = ut_asn1_ber_decoder_new(data6e);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6e), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6e));
  UtObjectRef data6f = ut_uint8_list_new_from_hex_string("0206ffffffffffff");
  UtObjectRef decoder6f = ut_asn1_ber_decoder_new(data6f);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6f), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6f));
  UtObjectRef data6g = ut_uint8_list_new_from_hex_string("0207ffffffffffffff");
  UtObjectRef decoder6g = ut_asn1_ber_decoder_new(data6g);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6g), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6g));
  UtObjectRef data6h =
      ut_uint8_list_new_from_hex_string("0208ffffffffffffffff");
  UtObjectRef decoder6h = ut_asn1_ber_decoder_new(data6h);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder6h), -1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6h));

  // Maximum unsigned value
  UtObjectRef data7 = ut_uint8_list_new_from_hex_string("02087fffffffffffffff");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder7), INT64_MAX);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder7));

  // Minimum signed value
  UtObjectRef data8 = ut_uint8_list_new_from_hex_string("02088000000000000000");
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder8), INT64_MIN);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder8));

  // 64 bit value.
  UtObjectRef data9 = ut_uint8_list_new_from_hex_string("02080123456789abcdef");
  UtObjectRef decoder9 = ut_asn1_ber_decoder_new(data9);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(decoder9),
                      0x0123456789abcdef);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder9));

  // Decoded as object with type.
  UtObjectRef data10 = ut_uint8_list_new_from_hex_string("02012a");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  UtObjectRef type10 = ut_asn1_integer_type_new();
  UtObjectRef value10 = ut_asn1_decoder_decode_value(decoder10, type10);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder10));
  ut_assert_true(ut_object_is_int64(value10));
  ut_assert_int_equal(ut_int64_get_value(value10), 42);

  // Values greater than 64 bit not supported.
  UtObjectRef data11 =
      ut_uint8_list_new_from_hex_string("0210ffffffffffffffffffffffffffffffff");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  ut_asn1_ber_decoder_decode_integer(decoder11);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder11),
                                      "Only 64 bit integers supported");

  // Empty data.
  UtObjectRef data12 = ut_uint8_list_new_from_hex_string("0200");
  UtObjectRef decoder12 = ut_asn1_ber_decoder_new(data12);
  ut_asn1_ber_decoder_decode_integer(decoder12);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder12),
                                      "Invalid INTEGER data length");

  // Constructed form.
  UtObjectRef data13 = ut_uint8_list_new_from_hex_string("220100");
  UtObjectRef decoder13 = ut_asn1_ber_decoder_new(data13);
  ut_asn1_ber_decoder_decode_integer(decoder13);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder13),
                                      "INTEGER does not have constructed form");
}

static void test_bit_string() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("030100");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_BIT_STRING);
  UtObjectRef string1 = ut_asn1_ber_decoder_decode_bit_string(decoder1);
  ut_assert_bit_list_equal_bin(string1, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("03020000");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef string2 = ut_asn1_ber_decoder_decode_bit_string(decoder2);
  ut_assert_bit_list_equal_bin(string2, "00000000");

  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("03020780");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef string3 = ut_asn1_ber_decoder_decode_bit_string(decoder3);
  ut_assert_bit_list_equal_bin(string3, "1");

  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("030204d0");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef string4 = ut_asn1_ber_decoder_decode_bit_string(decoder4);
  ut_assert_bit_list_equal_bin(string4, "1101");

  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("030500ff00f00f");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef string5 = ut_asn1_ber_decoder_decode_bit_string(decoder5);
  ut_assert_bit_list_equal_bin(string5, "11111111000000001111000000001111");

  // Decoded as object with type.
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("030204d0");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef type6 = ut_asn1_bit_string_type_new();
  UtObjectRef value6 = ut_asn1_decoder_decode_value(decoder6, type6);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6));
  ut_assert_true(ut_object_is_bit_list(value6));
  ut_assert_bit_list_equal_bin(value6, "1101");

  // Constructed form (not currently supported).
  UtObjectRef data11 =
      ut_uint8_list_new_from_hex_string("2308030204d0030204d0");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  UtObjectRef string11 = ut_asn1_ber_decoder_decode_bit_string(decoder11);
  // ut_assert_bit_list_equal_bin(string11, "11011101");
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder11),
                                      "Constructed BIT STRING not supported");
}

static void test_octet_string() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0400");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_OCTET_STRING);
  UtObjectRef string1 = ut_asn1_ber_decoder_decode_octet_string(decoder1);
  ut_assert_uint8_list_equal_hex(string1, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("040100");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef string2 = ut_asn1_ber_decoder_decode_octet_string(decoder2);
  ut_assert_uint8_list_equal_hex(string2, "00");

  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("04080123456789abcdef");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef string3 = ut_asn1_ber_decoder_decode_octet_string(decoder3);
  ut_assert_uint8_list_equal_hex(string3, "0123456789abcdef");

  // Maximum length in one byte.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("047f");
  ut_list_resize(data4, 2 + 127);
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef string4 = ut_asn1_ber_decoder_decode_octet_string(decoder4);
  ut_assert_int_equal(ut_list_get_length(string4), 127);

  // Minimum length in two bytes.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("04820080");
  ut_list_resize(data5, 4 + 128);
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef string5 = ut_asn1_ber_decoder_decode_octet_string(decoder5);
  ut_assert_int_equal(ut_list_get_length(string5), 128);

  // Maximum length in two bytes.
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("0482ffff");
  ut_list_resize(data6, 4 + 65535);
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef string6 = ut_asn1_ber_decoder_decode_octet_string(decoder6);
  ut_assert_int_equal(ut_list_get_length(string6), 65535);

  // Minimum length in three bytes.
  UtObjectRef data7 = ut_uint8_list_new_from_hex_string("0483010000");
  ut_list_resize(data7, 5 + 65536);
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  UtObjectRef string7 = ut_asn1_ber_decoder_decode_octet_string(decoder7);
  ut_assert_int_equal(ut_list_get_length(string7), 65536);

  // Maximum length in three bytes.
  UtObjectRef data8 = ut_uint8_list_new_from_hex_string("0483ffffff");
  ut_list_resize(data8, 5 + 16777215);
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  UtObjectRef string8 = ut_asn1_ber_decoder_decode_octet_string(decoder8);
  ut_assert_int_equal(ut_list_get_length(string8), 16777215);

  // Minimum length in four bytes.
  UtObjectRef data9 = ut_uint8_list_new_from_hex_string("048401000000");
  ut_list_resize(data9, 6 + 16777216);
  UtObjectRef decoder9 = ut_asn1_ber_decoder_new(data9);
  UtObjectRef string9 = ut_asn1_ber_decoder_decode_octet_string(decoder9);
  ut_assert_int_equal(ut_list_get_length(string9), 16777216);

  // Maximum length in four bytes.
  // UtObjectRef data10 = ut_uint8_list_new_from_hex_string("0484ffffffff");
  // ut_list_resize(data10, 6 + 4294967295);
  // UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  // UtObjectRef string10 = ut_asn1_ber_decoder_decode_octet_string(decoder10);
  // ut_assert_int_equal(ut_list_get_length(string10), 4294967295);

  // Decoded as object with type.
  UtObjectRef data11 =
      ut_uint8_list_new_from_hex_string("04080123456789abcdef");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  UtObjectRef type11 = ut_asn1_octet_string_type_new();
  UtObjectRef value11 = ut_asn1_decoder_decode_value(decoder11, type11);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder11));
  ut_assert_true(ut_object_implements_uint8_list(value11));
  ut_assert_uint8_list_equal_hex(value11, "0123456789abcdef");

  // Constructed form.
  UtObjectRef data12 =
      ut_uint8_list_new_from_hex_string("240c040401234567040489abcdef");
  UtObjectRef decoder12 = ut_asn1_ber_decoder_new(data12);
  UtObjectRef string12 = ut_asn1_ber_decoder_decode_octet_string(decoder12);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder12));
  ut_assert_uint8_list_equal_hex(string12, "0123456789abcdef");

  // FIXME: Invalid constructed forms
}

static void test_null() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0500");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_NULL);
  ut_asn1_ber_decoder_decode_null(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Decoded as object with type.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0500");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_null_type_new();
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_null(value2));

  // Too much data.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("050100");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_asn1_ber_decoder_decode_null(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid NULL data length");

  // Constructed form.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("2500");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_asn1_ber_decoder_decode_null(decoder4);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "NULL does not have constructed form");
}

static void test_object_identifier() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0603883703");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER);
  UtObjectRef id1 = ut_asn1_ber_decoder_decode_object_identifier(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  uint32_t expected_id1[] = {2, 999, 3};
  ut_assert_uint32_list_equal(id1, expected_id1, 3);

  // Decoded as object with type.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0603883703");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_object_identifier_type_new();
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_implements_uint32_list(value2));
  uint32_t expected_id2[] = {2, 999, 3};
  ut_assert_uint32_list_equal(id1, expected_id2, 3);

  // Empty.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("0600");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef id3 = ut_asn1_ber_decoder_decode_object_identifier(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid OBJECT IDENTIFIER");

  // Invalid integer.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("060188");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef id4 = ut_asn1_ber_decoder_decode_object_identifier(decoder4);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "Invalid OBJECT IDENTIFIER");

  // Constructed form.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("2603883703");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef id5 = ut_asn1_ber_decoder_decode_object_identifier(decoder5);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder5),
      "OBJECT IDENTIFIER does not have constructed form");
}

static void test_enumerated() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0a012a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_ENUMERATED);
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_enumerated(decoder1), 42);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // FIXME: Decoded as object with type.

  // Constructed form.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("2a012a");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_asn1_ber_decoder_decode_enumerated(decoder3);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder3),
      "ENUMERATED does not have constructed form");
}

static void test_utf8_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("0c0a48656c6c6f20f09f9880");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_UTF8_STRING);
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_utf8_string(decoder1);
  ut_assert_cstring_equal(string1, "Hello 😀");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0c00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_utf8_string(decoder2);
  ut_assert_cstring_equal(string2, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("2c0e0c0648656c6c6f200c04f09f9880");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_utf8_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello 😀");

  // FIXME: Decoded as object with type.

  // FIXME: Invalid UTF8
}

static void test_relative_oid() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0d04c27b0302");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID);
  UtObjectRef id1 = ut_asn1_ber_decoder_decode_relative_oid(decoder1);
  uint32_t expected_id1[] = {8571, 3, 2};
  ut_assert_uint32_list_equal(id1, expected_id1, 3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0d00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef id2 = ut_asn1_ber_decoder_decode_relative_oid(decoder2);
  uint32_t expected_id2[] = {};
  ut_assert_uint32_list_equal(id2, expected_id2, 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // Decoded as object with type.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("0d04c27b0302");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef type3 = ut_asn1_relative_oid_type_new();
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_implements_uint32_list(value3));
  uint32_t expected_id3[] = {8571, 3, 2};
  ut_assert_uint32_list_equal(id1, expected_id3, 3);

  // Invalid integer.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("0d0188");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef id4 = ut_asn1_ber_decoder_decode_relative_oid(decoder4);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "Invalid RELATIVE-OID");

  // Constructed form.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("2d04c27b0302");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef id5 = ut_asn1_ber_decoder_decode_relative_oid(decoder5);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder5),
      "RELATIVE-OID does not have constructed form");
}

static void test_sequence() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("30060101ff02012a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_SEQUENCE);
  UtObjectRef sequence1 = ut_asn1_ber_decoder_decode_sequence(decoder1);
  ut_assert_int_equal(ut_list_get_length(sequence1), 2);
  ut_assert_true(ut_asn1_ber_decoder_decode_boolean(
      ut_object_list_get_element(sequence1, 0)));
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_integer(
                          ut_object_list_get_element(sequence1, 1)),
                      42);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty sequence.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("3000");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef sequence2 = ut_asn1_ber_decoder_decode_sequence(decoder2);
  ut_assert_int_equal(ut_list_get_length(sequence2), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // Decoded as object with type.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("30100c0b4172746875722044656e7402012a");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef components3 = ut_map_new_ordered();
  ut_map_insert_string_take(components3, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components3, "age", ut_asn1_integer_type_new());
  UtObjectRef type3 = ut_asn1_sequence_type_new(components3, false);
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_implements_map(value3));
  ut_assert_int_equal(ut_map_get_length(value3), 2);
  UtObject *value3a = ut_map_lookup_string(value3, "name");
  ut_assert_true(ut_object_implements_string(value3a));
  ut_assert_cstring_equal(ut_string_get_text(value3a), "Arthur Dent");
  UtObject *value3b = ut_map_lookup_string(value3, "age");
  ut_assert_true(ut_object_is_int64(value3b));
  ut_assert_int_equal(ut_int64_get_value(value3b), 42);

  // Decoded as object with type (empty sequence).
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("3000");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef components4 = ut_map_new_ordered();
  UtObjectRef type4 = ut_asn1_sequence_type_new(components4, false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_map(value4));
  ut_assert_int_equal(ut_map_get_length(value4), 0);

  // FIXME: Optional components.
  // FIXME: Default components.
  // FIXME: Extended type with additional components.

  // Non-constructed form.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("1000");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef sequence5 = ut_asn1_ber_decoder_decode_sequence(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Sequence must be constructed");

  // Missing component (age).
  UtObjectRef data6 =
      ut_uint8_list_new_from_hex_string("300d0c0b4172746875722044656e74");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef components6 = ut_map_new_ordered();
  ut_map_insert_string_take(components6, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components6, "age", ut_asn1_integer_type_new());
  UtObjectRef type6 = ut_asn1_sequence_type_new(components6, false);
  UtObjectRef value6 = ut_asn1_decoder_decode_value(decoder6, type6);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder6),
                                      "Required SEQUENCE components missing");
}

static void test_sequence_of() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("3009020101020102020103");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  UtObjectRef child_type1 = ut_asn1_integer_type_new();
  UtObjectRef type1 = ut_asn1_sequence_of_type_new(child_type1);
  UtObjectRef value1 = ut_asn1_decoder_decode_value(decoder1, type1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_true(ut_object_implements_list(value1));
  ut_assert_int_equal(ut_list_get_length(value1), 3);
  UtObject *value1a = ut_object_list_get_element(value1, 0);
  ut_assert_true(ut_object_is_int64(value1a));
  ut_assert_int_equal(ut_int64_get_value(value1a), 1);
  UtObject *value1b = ut_object_list_get_element(value1, 1);
  ut_assert_true(ut_object_is_int64(value1b));
  ut_assert_int_equal(ut_int64_get_value(value1b), 2);
  UtObject *value1c = ut_object_list_get_element(value1, 2);
  ut_assert_true(ut_object_is_int64(value1c));
  ut_assert_int_equal(ut_int64_get_value(value1c), 3);

  // Empty list.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("3000");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef child_type2 = ut_asn1_integer_type_new();
  UtObjectRef type2 = ut_asn1_sequence_of_type_new(child_type2);
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_implements_list(value2));
  ut_assert_int_equal(ut_list_get_length(value2), 0);
}

static void test_set() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("31060101ff02012a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_SET);
  UtObjectRef set1 = ut_asn1_ber_decoder_decode_set(decoder1);
  ut_assert_int_equal(ut_list_get_length(set1), 2);
  ut_assert_true(
      ut_asn1_ber_decoder_decode_boolean(ut_object_list_get_element(set1, 0)));
  ut_assert_int_equal(
      ut_asn1_ber_decoder_decode_integer(ut_object_list_get_element(set1, 1)),
      42);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty set.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("3100");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef set2 = ut_asn1_ber_decoder_decode_set(decoder2);
  ut_assert_int_equal(ut_list_get_length(set2), 0);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // Decoded as object with type.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("31100c0b4172746875722044656e7402012a");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef components3 = ut_map_new_ordered();
  ut_map_insert_string_take(components3, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components3, "age", ut_asn1_integer_type_new());
  UtObjectRef type3 = ut_asn1_set_type_new(components3, false);
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_implements_map(value3));
  ut_assert_int_equal(ut_map_get_length(value3), 2);
  UtObject *value3a = ut_map_lookup_string(value3, "name");
  ut_assert_true(ut_object_implements_string(value3a));
  ut_assert_cstring_equal(ut_string_get_text(value3a), "Arthur Dent");
  UtObject *value3b = ut_map_lookup_string(value3, "age");
  ut_assert_true(ut_object_is_int64(value3b));
  ut_assert_int_equal(ut_int64_get_value(value3b), 42);

  // Decoded as object with type (empty set).
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("3100");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef components4 = ut_map_new_ordered();
  UtObjectRef type4 = ut_asn1_set_type_new(components4, false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_map(value4));
  ut_assert_int_equal(ut_map_get_length(value4), 0);

  // Non-constructed form.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("1100");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef set5 = ut_asn1_ber_decoder_decode_set(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Set must be constructed");

  // Missing component (age).
  UtObjectRef data6 =
      ut_uint8_list_new_from_hex_string("310d0c0b4172746875722044656e74");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef components6 = ut_map_new_ordered();
  ut_map_insert_string_take(components6, "name",
                            ut_asn1_utf8_string_type_new());
  ut_map_insert_string_take(components6, "age", ut_asn1_integer_type_new());
  UtObjectRef type6 = ut_asn1_set_type_new(components6, false);
  UtObjectRef value6 = ut_asn1_decoder_decode_value(decoder6, type6);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder6),
                                      "Required SET components missing");
}

static void test_set_of() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("3109020101020102020103");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  UtObjectRef child_type1 = ut_asn1_integer_type_new();
  UtObjectRef type1 = ut_asn1_set_of_type_new(child_type1);
  UtObjectRef value1 = ut_asn1_decoder_decode_value(decoder1, type1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_true(ut_object_implements_list(value1));
  ut_assert_int_equal(ut_list_get_length(value1), 3);
  UtObject *value1a = ut_object_list_get_element(value1, 0);
  ut_assert_true(ut_object_is_int64(value1a));
  ut_assert_int_equal(ut_int64_get_value(value1a), 1);
  UtObject *value1b = ut_object_list_get_element(value1, 1);
  ut_assert_true(ut_object_is_int64(value1b));
  ut_assert_int_equal(ut_int64_get_value(value1b), 2);
  UtObject *value1c = ut_object_list_get_element(value1, 2);
  ut_assert_true(ut_object_is_int64(value1c));
  ut_assert_int_equal(ut_int64_get_value(value1c), 3);

  // Empty list.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("3100");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef child_type2 = ut_asn1_integer_type_new();
  UtObjectRef type2 = ut_asn1_set_of_type_new(child_type2);
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_implements_list(value2));
  ut_assert_int_equal(ut_list_get_length(value2), 0);
}

static void test_numeric_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("120b3132333435203637383930");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING);
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_numeric_string(decoder1);
  ut_assert_cstring_equal(string1, "12345 67890");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1200");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_numeric_string(decoder2);
  ut_assert_cstring_equal(string2, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // FIXME: Decoded as object with type.

  // Invalid characters - "Hello World"
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("120b48656c6c6f20576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_numeric_string(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid numeric string");

  // Constructed form (not currently supported).
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("320f120631323334352012053637383930");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_cstring_ref string4 = ut_asn1_ber_decoder_decode_numeric_string(decoder4);
  // ut_assert_cstring_equal(string4, "12345 67890");
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder4),
      "Constructed numeric string not supported");
}

static void test_printable_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("130b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING);
  ut_cstring_ref string1 =
      ut_asn1_ber_decoder_decode_printable_string(decoder1);
  ut_assert_cstring_equal(string1, "Hello World");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1300");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 =
      ut_asn1_ber_decoder_decode_printable_string(decoder2);
  ut_assert_cstring_equal(string2, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // FIXME: Decoded as object with type.

  // Invalid characters - "#invalid"
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("130823696e76616c696423");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 =
      ut_asn1_ber_decoder_decode_printable_string(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid printable string");

  // Constructed form (not currently supported).
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("330f130648656c6c6f201306576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_cstring_ref string4 =
      ut_asn1_ber_decoder_decode_printable_string(decoder4);
  // ut_assert_cstring_equal(string4, "Hello World");
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder4),
      "Constructed printable string not supported");
}

static void test_ia5_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("160b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_IA5_STRING);
  UtObjectRef string1 = ut_asn1_ber_decoder_decode_ia5_string(decoder1);
  ut_assert_cstring_equal(ut_string_get_text(string1), "Hello World");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1600");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef string2 = ut_asn1_ber_decoder_decode_ia5_string(decoder2);
  ut_assert_cstring_equal(ut_string_get_text(string2), "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // FIXME: Decoded as object with type.

  // Invalid characters (>7 bits).
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("1601ff");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef string3 = ut_asn1_ber_decoder_decode_ia5_string(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid IA5 string");

  // Constructed form (not currently supported).
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("360f160648656c6c6f201606576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef string4 = ut_asn1_ber_decoder_decode_ia5_string(decoder4);
  // ut_assert_cstring_equal(string4, "Hello World");
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "Constructed IA5 string not supported");
}

static void test_visible_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("1a0b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_tag_class(decoder1),
                      UT_ASN1_TAG_CLASS_UNIVERSAL);
  ut_assert_int_equal(ut_asn1_ber_decoder_get_identifier_number(decoder1),
                      UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING);
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_visible_string(decoder1);
  ut_assert_cstring_equal(string1, "Hello World");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1a00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_visible_string(decoder2);
  ut_assert_cstring_equal(string2, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));

  // FIXME: Decoded as object with type.

  // Invalid characters (control character).
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("160101");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_visible_string(decoder3);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder3),
                                      "Invalid visible string");

  // Constructed form (not currently supported).
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("3a0f1a0648656c6c6f201a06576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_cstring_ref string4 = ut_asn1_ber_decoder_decode_visible_string(decoder4);
  // ut_assert_cstring_equal(string4, "Hello World");
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder4),
      "Constructed visible string not supported");
}

static void test_choice() {
  // FIXME
}

static void test_tagged_type() {
  // FIXME
}

int main(int argc, char **argv) {
  test_boolean();
  test_integer();
  test_bit_string();
  test_octet_string();
  test_null();
  test_object_identifier();
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
