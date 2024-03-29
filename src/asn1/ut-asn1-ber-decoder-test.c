#include <assert.h>
#include <float.h>
#include <math.h>

#include "ut.h"

static void test_boolean() {
  // False is zero.
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("010100");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_BOOLEAN));
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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1a),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_INTEGER));
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
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder11),
      "INTEGER greater than 64 bits not supported");

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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_BIT_STRING));
  UtObjectRef string1 = ut_asn1_ber_decoder_decode_bit_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_bit_list_equal_bin(string1, "");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("03020000");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef string2 = ut_asn1_ber_decoder_decode_bit_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_bit_list_equal_bin(string2, "00000000");

  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("03020780");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef string3 = ut_asn1_ber_decoder_decode_bit_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_bit_list_equal_bin(string3, "1");

  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("030204d0");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef string4 = ut_asn1_ber_decoder_decode_bit_string(decoder4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_bit_list_equal_bin(string4, "1101");

  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("030500ff00f00f");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef string5 = ut_asn1_ber_decoder_decode_bit_string(decoder5);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder5));
  ut_assert_bit_list_equal_bin(string5, "11111111000000001111000000001111");

  // Constructed form.
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("2308030204d0030204d0");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef string6 = ut_asn1_ber_decoder_decode_bit_string(decoder6);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6));
  ut_assert_bit_list_equal_bin(string6, "11011101");

  // Decoded as object with type.
  UtObjectRef data7 = ut_uint8_list_new_from_hex_string("030204d0");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  UtObjectRef type7 = ut_asn1_bit_string_type_new();
  UtObjectRef value7 = ut_asn1_decoder_decode_value(decoder7, type7);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder7));
  ut_assert_true(ut_object_is_bit_list(value7));
  ut_assert_bit_list_equal_bin(value7, "1101");

  // Invalid number of unused bits.
  UtObjectRef data8 = ut_uint8_list_new_from_hex_string("030508ff00f00f");
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  UtObjectRef string8 = ut_asn1_ber_decoder_decode_bit_string(decoder8);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder8),
      "Invalid number of unused bits in BIT STRING");

  // Invalid constructed form - wrong type.
  UtObjectRef data14 = ut_uint8_list_new_from_hex_string("230301012a");
  UtObjectRef decoder14 = ut_asn1_ber_decoder_new(data14);
  UtObjectRef string14 = ut_asn1_ber_decoder_decode_bit_string(decoder14);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder14),
      "Invalid tag inside constructed BIT STRING");

  // Invalid constructed form - nested.
  UtObjectRef data15 = ut_uint8_list_new_from_hex_string("23062304030204d0");
  UtObjectRef decoder15 = ut_asn1_ber_decoder_new(data15);
  UtObjectRef string15 = ut_asn1_ber_decoder_decode_bit_string(decoder15);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder15),
                                      "Invalid nested constructed BIT STRING");

  // Invalid constructed form - insufficient data in child element.
  UtObjectRef data16 = ut_uint8_list_new_from_hex_string("2304030304d0");
  UtObjectRef decoder16 = ut_asn1_ber_decoder_new(data16);
  UtObjectRef string16 = ut_asn1_ber_decoder_decode_bit_string(decoder16);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder16),
      "Error decoding constructed BIT STRING element: Insufficient data");
}

static void test_octet_string() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0400");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_OCTET_STRING));
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

  // Insufficient data.
  UtObjectRef data13 = ut_uint8_list_new_from_hex_string("0408dead");
  UtObjectRef decoder13 = ut_asn1_ber_decoder_new(data13);
  UtObjectRef string13 = ut_asn1_ber_decoder_decode_octet_string(decoder13);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder13),
                                      "Insufficient data");

  // Invalid constructed form - wrong type.
  UtObjectRef data14 = ut_uint8_list_new_from_hex_string("240301012a");
  UtObjectRef decoder14 = ut_asn1_ber_decoder_new(data14);
  UtObjectRef string14 = ut_asn1_ber_decoder_decode_octet_string(decoder14);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder14),
      "Invalid tag inside constructed OCTET STRING");

  // Invalid constructed form - nested.
  UtObjectRef data15 = ut_uint8_list_new_from_hex_string("240624040402dead");
  UtObjectRef decoder15 = ut_asn1_ber_decoder_new(data15);
  UtObjectRef string15 = ut_asn1_ber_decoder_decode_octet_string(decoder15);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder15),
      "Invalid nested constructed OCTET STRING");

  // Invalid constructed form - insufficient data in child element.
  UtObjectRef data16 = ut_uint8_list_new_from_hex_string("24040403dead");
  UtObjectRef decoder16 = ut_asn1_ber_decoder_new(data16);
  UtObjectRef string16 = ut_asn1_ber_decoder_decode_octet_string(decoder16);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder16),
      "Error decoding constructed OCTET STRING element: Insufficient data");
}

static void test_null() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0500");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_NULL));
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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_OBJECT_IDENTIFIER));
  UtObjectRef id1 = ut_asn1_ber_decoder_decode_object_identifier(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_cstring_ref id_string1 = ut_object_identifier_to_string(id1);
  ut_assert_cstring_equal(id_string1, "2.999.3");

  // Decoded as object with type.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0603883703");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_object_identifier_type_new();
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_object_identifier(value2));
  ut_cstring_ref value_string2 = ut_object_identifier_to_string(value2);
  ut_assert_cstring_equal(value_string2, "2.999.3");

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

static void test_object_descriptor() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("070e4578616d706c65204f626a656374");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_OBJECT_DESCRIPTOR));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_graphic_string(decoder1);
  ut_assert_cstring_equal(string1, "Example Object");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0700");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_graphic_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string(
      "271207084578616d706c652007064f626a656374");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_graphic_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Example Object");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("070e4578616d706c65204f626a656374");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_object_descriptor_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Example Object");

  // Invalid characters - "Example\tObject".
  UtObjectRef data5 =
      ut_uint8_list_new_from_hex_string("070e4578616d706c65094f626a656374");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_cstring_ref string5 =
      ut_asn1_ber_decoder_decode_object_descriptor(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid ObjectDescriptor");
}

static void test_external() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("280ba005a20302012aa2020400");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_EXTERNAL));
  UtObjectRef value1 = ut_asn1_ber_decoder_decode_external(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Decoded as object with type.
  UtObjectRef data2 =
      ut_uint8_list_new_from_hex_string("280ba005a20302012aa2020400");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_external_type_new();
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_asn1_embedded_value(value2));

  // Unsupported identification type (fixed).
  UtObjectRef data10 =
      ut_uint8_list_new_from_hex_string("280aa004a5020500a2020400");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  UtObjectRef value10 = ut_asn1_ber_decoder_decode_external(decoder10);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder10),
      "Unsupported identification for EXTERNAL type");
}

static void test_real() {
  // Zero.
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0900");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_REAL));
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder1), 0.0);

  // Integer.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0903800105");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder2), 10.0);

  // Negative number.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("0903c00105");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder3), -10.0);

  // Pi.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("090980ce0c90fcf80dc337");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder4), 3.14159);

  // Largest double.
  UtObjectRef data5 =
      ut_uint8_list_new_from_hex_string("090a8103cb1fffffffffffff");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder5), DBL_MAX);

  // Smallest double.
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("090481fc0201");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder6), DBL_MIN);

  // Infinity.
  UtObjectRef data10 = ut_uint8_list_new_from_hex_string("090140");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder10), INFINITY);

  // Minus infinity.
  UtObjectRef data11 = ut_uint8_list_new_from_hex_string("090141");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder11), -INFINITY);

  // Not a number.
  UtObjectRef data12 = ut_uint8_list_new_from_hex_string("090142");
  UtObjectRef decoder12 = ut_asn1_ber_decoder_new(data12);
  ut_assert_true(isnan(ut_asn1_ber_decoder_decode_real(decoder12)));

  // Negative zero.
  UtObjectRef data13 = ut_uint8_list_new_from_hex_string("090143");
  UtObjectRef decoder13 = ut_asn1_ber_decoder_new(data13);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder13), -0.0);

  // Base 8 number.
  UtObjectRef data14 = ut_uint8_list_new_from_hex_string("0903900105");
  UtObjectRef decoder14 = ut_asn1_ber_decoder_new(data14);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder14), 40.0);

  // Base 16 number.
  UtObjectRef data15 = ut_uint8_list_new_from_hex_string("0903a00105");
  UtObjectRef decoder15 = ut_asn1_ber_decoder_new(data15);
  ut_assert_float_equal(ut_asn1_ber_decoder_decode_real(decoder15), 80.0);

  // Decoded as object with type.
  UtObjectRef data20 = ut_uint8_list_new_from_hex_string("090380000a");
  UtObjectRef decoder20 = ut_asn1_ber_decoder_new(data20);
  UtObjectRef type20 = ut_asn1_real_type_new();
  UtObjectRef value20 = ut_asn1_decoder_decode_value(decoder20, type20);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder20));
  ut_assert_true(ut_object_is_float64(value20));
  ut_assert_float_equal(ut_float64_get_value(value20), 10.0);

  // Constructed form.
  UtObjectRef data21 = ut_uint8_list_new_from_hex_string("290380000a");
  UtObjectRef decoder21 = ut_asn1_ber_decoder_new(data21);
  ut_asn1_ber_decoder_decode_real(decoder21);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder21),
                                      "REAL does not have constructed form");

  // Invalid special value.
  UtObjectRef data22 = ut_uint8_list_new_from_hex_string("09014f");
  UtObjectRef decoder22 = ut_asn1_ber_decoder_new(data22);
  ut_asn1_ber_decoder_decode_real(decoder22);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder22),
                                      "Invalid REAL special value");

  // Invalid base.
  UtObjectRef data23 = ut_uint8_list_new_from_hex_string("0903b00105");
  UtObjectRef decoder23 = ut_asn1_ber_decoder_new(data23);
  ut_asn1_ber_decoder_decode_real(decoder23);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder23),
                                      "Invalid REAL base");

  // ISO 6093 NR1 decimal encoding.
  UtObjectRef data24 = ut_uint8_list_new_from_hex_string("09020131");
  UtObjectRef decoder24 = ut_asn1_ber_decoder_new(data24);
  ut_asn1_ber_decoder_decode_real(decoder24);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder24),
      "REAL ISO 6093 NR1 decimal encoding not supported");

  // ISO 6093 NR2 decimal encoding.
  UtObjectRef data25 = ut_uint8_list_new_from_hex_string("09020231");
  UtObjectRef decoder25 = ut_asn1_ber_decoder_new(data25);
  ut_asn1_ber_decoder_decode_real(decoder25);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder25),
      "REAL ISO 6093 NR2 decimal encoding not supported");

  // ISO 6093 NR3 decimal encoding.
  UtObjectRef data26 = ut_uint8_list_new_from_hex_string("09020331");
  UtObjectRef decoder26 = ut_asn1_ber_decoder_new(data26);
  ut_asn1_ber_decoder_decode_real(decoder26);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder26),
      "REAL ISO 6093 NR3 decimal encoding not supported");

  // Invalid decimal encoding.
  UtObjectRef data27 = ut_uint8_list_new_from_hex_string("09023f31");
  UtObjectRef decoder27 = ut_asn1_ber_decoder_new(data27);
  ut_asn1_ber_decoder_decode_real(decoder27);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder27),
                                      "Invalid REAL decimal encoding");

  // Special value with additional data.
  UtObjectRef data28 = ut_uint8_list_new_from_hex_string("09024fff");
  UtObjectRef decoder28 = ut_asn1_ber_decoder_new(data28);
  ut_asn1_ber_decoder_decode_real(decoder28);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder28),
                                      "Invalid length REAL special value");

  // Insufficient space for exponent.
  UtObjectRef data29 = ut_uint8_list_new_from_hex_string("090180");
  UtObjectRef decoder29 = ut_asn1_ber_decoder_new(data29);
  ut_asn1_ber_decoder_decode_real(decoder29);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder29),
                                      "Insufficient space for REAL exponent");

  // Insufficient space for variable length exponent.
  UtObjectRef data30 = ut_uint8_list_new_from_hex_string("09028301");
  UtObjectRef decoder30 = ut_asn1_ber_decoder_new(data30);
  ut_asn1_ber_decoder_decode_real(decoder30);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder30),
                                      "Insufficient space for REAL exponent");

  // Invalid variable exponent length.
  UtObjectRef data31 = ut_uint8_list_new_from_hex_string("09028300");
  UtObjectRef decoder31 = ut_asn1_ber_decoder_new(data31);
  ut_asn1_ber_decoder_decode_real(decoder31);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder31),
                                      "Invalid REAL exponent length");

  // Variable exponent > 32 bits.
  UtObjectRef data32 = ut_uint8_list_new_from_hex_string("09028305ffffffff");
  UtObjectRef decoder32 = ut_asn1_ber_decoder_new(data32);
  ut_asn1_ber_decoder_decode_real(decoder32);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder32),
                                      "Unsupported REAL exponent length");
}

static void test_enumerated() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0a012a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_ENUMERATED));
  ut_assert_int_equal(ut_asn1_ber_decoder_decode_enumerated(decoder1), 42);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Decoded as object with type.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("0a0102");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2_items = ut_map_new_string_from_elements_take(
      "red", ut_uint64_new(1), "green", ut_uint64_new(2), "blue",
      ut_uint64_new(3), NULL);
  UtObjectRef type2 = ut_asn1_enumerated_type_new(type2_items, false);
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_implements_string(value2));
  ut_assert_cstring_equal(ut_string_get_text(value2), "green");

  // Unknown enumeration item (extensible).
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("0a0104");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef type3_items = ut_map_new_string_from_elements_take(
      "red", ut_uint64_new(1), "green", ut_uint64_new(2), "blue",
      ut_uint64_new(3), NULL);
  UtObjectRef type3 = ut_asn1_enumerated_type_new(type3_items, true);
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_implements_string(value3));
  ut_assert_cstring_equal(ut_string_get_text(value3), "4");

  // Unknown enumeration item (not extensible).
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("0a0104");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4_items = ut_map_new_string_from_elements_take(
      "red", ut_uint64_new(1), "green", ut_uint64_new(2), "blue",
      ut_uint64_new(3), NULL);
  UtObjectRef type4 = ut_asn1_enumerated_type_new(type4_items, false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "Unknown enumeration value 4");

  // Constructed form.
  UtObjectRef data10 = ut_uint8_list_new_from_hex_string("2a012a");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  ut_asn1_ber_decoder_decode_enumerated(decoder10);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder10),
      "ENUMERATED does not have constructed form");
}

static void test_embedded_pdv() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("2b0aa004a5020500a2020400");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_EMBEDDED_PDV));
  UtObjectRef value1 = ut_asn1_ber_decoder_decode_embedded_pdv(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Decoded as object with type.
  UtObjectRef data2 =
      ut_uint8_list_new_from_hex_string("2b0aa004a5020500a2020400");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_embedded_pdv_type_new();
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_asn1_embedded_value(value2));
}

static void test_utf8_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("0c0a48656c6c6f20f09f9880");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_UTF8_STRING));
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

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("0c0a48656c6c6f20f09f9880");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_utf8_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello 😀");

  // FIXME: Invalid UTF8
}

static void test_relative_oid() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("0d04c27b0302");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_RELATIVE_OID));
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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_SEQUENCE));
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
  UtObjectRef components3 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
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
  UtObjectRef type4 = ut_asn1_sequence_type_new_take(ut_map_new(), false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_map(value4));
  ut_assert_int_equal(ut_map_get_length(value4), 0);

  // Additional component (extensible).
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string(
      "30130c0b4172746875722044656e7402012a0101ff");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef components5 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type5 = ut_asn1_sequence_type_new(components5, true);
  UtObjectRef value5 = ut_asn1_decoder_decode_value(decoder5, type5);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder5));
  ut_assert_int_equal(ut_map_get_length(value5), 2);
  UtObject *value5a = ut_map_lookup_string(value5, "name");
  ut_assert_true(ut_object_implements_string(value5a));
  ut_assert_cstring_equal(ut_string_get_text(value5a), "Arthur Dent");
  UtObject *value5b = ut_map_lookup_string(value5, "age");
  ut_assert_true(ut_object_is_int64(value5b));
  ut_assert_int_equal(ut_int64_get_value(value5b), 42);

  // Optional component (present).
  UtObjectRef data6 =
      ut_uint8_list_new_from_hex_string("30100c0b4172746875722044656e7402012a");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef components6 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type6 = ut_asn1_sequence_type_new(components6, false);
  UtObjectRef value6 = ut_asn1_decoder_decode_value(decoder6, type6);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6));
  ut_assert_true(ut_object_implements_map(value6));
  ut_assert_int_equal(ut_map_get_length(value6), 2);
  UtObject *value6a = ut_map_lookup_string(value6, "name");
  ut_assert_true(ut_object_implements_string(value6a));
  ut_assert_cstring_equal(ut_string_get_text(value6a), "Arthur Dent");
  UtObject *value6b = ut_map_lookup_string(value6, "age");
  ut_assert_true(ut_object_is_int64(value6b));
  ut_assert_int_equal(ut_int64_get_value(value6b), 42);

  // Optional component (not present).
  UtObjectRef data7 =
      ut_uint8_list_new_from_hex_string("300d0c0b4172746875722044656e74");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  UtObjectRef components7 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type7 = ut_asn1_sequence_type_new(components7, false);
  UtObjectRef value7 = ut_asn1_decoder_decode_value(decoder7, type7);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder7));
  ut_assert_true(ut_object_implements_map(value7));
  ut_assert_int_equal(ut_map_get_length(value7), 1);
  UtObject *value7a = ut_map_lookup_string(value7, "name");
  ut_assert_true(ut_object_implements_string(value7a));
  ut_assert_cstring_equal(ut_string_get_text(value7a), "Arthur Dent");
  UtObject *value7b = ut_map_lookup_string(value7, "age");
  ut_assert_null_object(value7b);

  // Optional components (none present).
  UtObjectRef data8 = ut_uint8_list_new_from_hex_string("3000");
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  UtObjectRef components8 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type8 = ut_asn1_sequence_type_new(components8, false);
  UtObjectRef value8 = ut_asn1_decoder_decode_value(decoder8, type8);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder8));
  ut_assert_true(ut_object_implements_map(value8));
  ut_assert_int_equal(ut_map_get_length(value8), 0);

  // Optional components (first present).
  UtObjectRef data9 =
      ut_uint8_list_new_from_hex_string("300d0c0b4172746875722044656e74");
  UtObjectRef decoder9 = ut_asn1_ber_decoder_new(data9);
  UtObjectRef components9 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type9 = ut_asn1_sequence_type_new(components9, false);
  UtObjectRef value9 = ut_asn1_decoder_decode_value(decoder9, type9);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder9));
  ut_assert_true(ut_object_implements_map(value9));
  ut_assert_int_equal(ut_map_get_length(value9), 1);
  UtObject *value9a = ut_map_lookup_string(value9, "name");
  ut_assert_true(ut_object_implements_string(value9a));
  ut_assert_cstring_equal(ut_string_get_text(value9a), "Arthur Dent");
  UtObject *value9b = ut_map_lookup_string(value9, "age");
  ut_assert_null_object(value9b);

  // Optional components (second present).
  UtObjectRef data10 = ut_uint8_list_new_from_hex_string("300302012a");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  UtObjectRef components10 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type10 = ut_asn1_sequence_type_new(components10, false);
  UtObjectRef value10 = ut_asn1_decoder_decode_value(decoder10, type10);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder10));
  ut_assert_true(ut_object_implements_map(value10));
  ut_assert_int_equal(ut_map_get_length(value10), 1);
  UtObject *value10a = ut_map_lookup_string(value10, "name");
  ut_assert_null_object(value10a);
  UtObject *value10b = ut_map_lookup_string(value10, "age");
  ut_assert_true(ut_object_is_int64(value10b));
  ut_assert_int_equal(ut_int64_get_value(value10b), 42);

  // Default component (present).
  UtObjectRef data11 =
      ut_uint8_list_new_from_hex_string("30100c0b4172746875722044656e7402012a");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  UtObjectRef components11 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type11 = ut_asn1_sequence_type_new(components11, false);
  UtObjectRef value11 = ut_asn1_decoder_decode_value(decoder11, type11);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder11));
  ut_assert_true(ut_object_implements_map(value11));
  ut_assert_int_equal(ut_map_get_length(value11), 2);
  UtObject *value11a = ut_map_lookup_string(value11, "name");
  ut_assert_true(ut_object_implements_string(value11a));
  ut_assert_cstring_equal(ut_string_get_text(value11a), "Arthur Dent");
  UtObject *value11b = ut_map_lookup_string(value11, "age");
  ut_assert_true(ut_object_is_int64(value11b));
  ut_assert_int_equal(ut_int64_get_value(value11b), 42);

  // Default component (present with default value).
  UtObjectRef data12 =
      ut_uint8_list_new_from_hex_string("30100c0b4172746875722044656e74020163");
  UtObjectRef decoder12 = ut_asn1_ber_decoder_new(data12);
  UtObjectRef components12 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type12 = ut_asn1_sequence_type_new(components12, false);
  UtObjectRef value12 = ut_asn1_decoder_decode_value(decoder12, type12);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder12));
  ut_assert_true(ut_object_implements_map(value12));
  ut_assert_int_equal(ut_map_get_length(value12), 2);
  UtObject *value12a = ut_map_lookup_string(value12, "name");
  ut_assert_true(ut_object_implements_string(value12a));
  ut_assert_cstring_equal(ut_string_get_text(value12a), "Arthur Dent");
  UtObject *value12b = ut_map_lookup_string(value12, "age");
  ut_assert_true(ut_object_is_int64(value12b));
  ut_assert_int_equal(ut_int64_get_value(value12b), 99);

  // Default component (not present).
  UtObjectRef data13 =
      ut_uint8_list_new_from_hex_string("300d0c0b4172746875722044656e74");
  UtObjectRef decoder13 = ut_asn1_ber_decoder_new(data13);
  UtObjectRef components13 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type13 = ut_asn1_sequence_type_new(components13, false);
  UtObjectRef value13 = ut_asn1_decoder_decode_value(decoder13, type13);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder13));
  ut_assert_true(ut_object_implements_map(value13));
  ut_assert_int_equal(ut_map_get_length(value13), 2);
  UtObject *value13a = ut_map_lookup_string(value13, "name");
  ut_assert_true(ut_object_implements_string(value13a));
  ut_assert_cstring_equal(ut_string_get_text(value13a), "Arthur Dent");
  UtObject *value13b = ut_map_lookup_string(value13, "age");
  ut_assert_true(ut_object_is_int64(value13b));
  ut_assert_int_equal(ut_int64_get_value(value13b), 99);

  // Non-constructed form.
  UtObjectRef data20 = ut_uint8_list_new_from_hex_string("1000");
  UtObjectRef decoder20 = ut_asn1_ber_decoder_new(data20);
  UtObjectRef sequence20 = ut_asn1_ber_decoder_decode_sequence(decoder20);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder20),
                                      "Sequence must be constructed");

  // Missing component (age).
  UtObjectRef data21 =
      ut_uint8_list_new_from_hex_string("300d0c0b4172746875722044656e74");
  UtObjectRef decoder21 = ut_asn1_ber_decoder_new(data21);
  UtObjectRef components21 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type21 = ut_asn1_sequence_type_new(components21, false);
  UtObjectRef value21 = ut_asn1_decoder_decode_value(decoder21, type21);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder21),
                                      "Required SEQUENCE components missing");

  // Components in reverse order.
  UtObjectRef data22 =
      ut_uint8_list_new_from_hex_string("301002012a0c0b4172746875722044656e74");
  UtObjectRef decoder22 = ut_asn1_ber_decoder_new(data22);
  UtObjectRef components22 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type22 = ut_asn1_sequence_type_new(components22, false);
  UtObjectRef value22 = ut_asn1_decoder_decode_value(decoder22, type22);
  ut_assert_is_error_with_description(
      ut_asn1_decoder_get_error(decoder22),
      "Required SEQUENCE component name missing");

  // Additional component (not extensible).
  UtObjectRef data23 = ut_uint8_list_new_from_hex_string(
      "30130c0b4172746875722044656e7402012a0101ff");
  UtObjectRef decoder23 = ut_asn1_ber_decoder_new(data23);
  UtObjectRef components23 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type23 = ut_asn1_sequence_type_new(components23, false);
  UtObjectRef value23 = ut_asn1_decoder_decode_value(decoder23, type23);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder23),
                                      "Too many SEQUENCE components");
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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_SET));
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
  UtObjectRef components3 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
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
  UtObjectRef type4 = ut_asn1_set_type_new_take(ut_map_new(), false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_map(value4));
  ut_assert_int_equal(ut_map_get_length(value4), 0);

  // Components in reverse order.
  UtObjectRef data5 =
      ut_uint8_list_new_from_hex_string("311002012a0c0b4172746875722044656e74");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef components5 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type5 = ut_asn1_set_type_new(components5, false);
  UtObjectRef value5 = ut_asn1_decoder_decode_value(decoder5, type5);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder5));
  ut_assert_true(ut_object_implements_map(value5));
  ut_assert_int_equal(ut_map_get_length(value5), 2);
  UtObject *value5a = ut_map_lookup_string(value5, "name");
  ut_assert_true(ut_object_implements_string(value5a));
  ut_assert_cstring_equal(ut_string_get_text(value5a), "Arthur Dent");
  UtObject *value5b = ut_map_lookup_string(value5, "age");
  ut_assert_true(ut_object_is_int64(value5b));
  ut_assert_int_equal(ut_int64_get_value(value5b), 42);

  // Additional component (extensible).
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string(
      "31130c0b4172746875722044656e7402012a0101ff");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef components6 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type6 = ut_asn1_set_type_new(components6, true);
  UtObjectRef value6 = ut_asn1_decoder_decode_value(decoder6, type6);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder6));
  ut_assert_int_equal(ut_map_get_length(value6), 2);
  UtObject *value6a = ut_map_lookup_string(value6, "name");
  ut_assert_true(ut_object_implements_string(value6a));
  ut_assert_cstring_equal(ut_string_get_text(value6a), "Arthur Dent");
  UtObject *value6b = ut_map_lookup_string(value6, "age");
  ut_assert_true(ut_object_is_int64(value6b));
  ut_assert_int_equal(ut_int64_get_value(value6b), 42);

  // Optional component (present).
  UtObjectRef data7 =
      ut_uint8_list_new_from_hex_string("31100c0b4172746875722044656e7402012a");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  UtObjectRef components7 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type7 = ut_asn1_set_type_new(components7, false);
  UtObjectRef value7 = ut_asn1_decoder_decode_value(decoder7, type7);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder7));
  ut_assert_true(ut_object_implements_map(value7));
  ut_assert_int_equal(ut_map_get_length(value7), 2);
  UtObject *value7a = ut_map_lookup_string(value7, "name");
  ut_assert_true(ut_object_implements_string(value7a));
  ut_assert_cstring_equal(ut_string_get_text(value7a), "Arthur Dent");
  UtObject *value7b = ut_map_lookup_string(value7, "age");
  ut_assert_true(ut_object_is_int64(value7b));
  ut_assert_int_equal(ut_int64_get_value(value7b), 42);

  // Optional component (not present).
  UtObjectRef data8 =
      ut_uint8_list_new_from_hex_string("310d0c0b4172746875722044656e74");
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  UtObjectRef components8 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type8 = ut_asn1_set_type_new(components8, false);
  UtObjectRef value8 = ut_asn1_decoder_decode_value(decoder8, type8);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder8));
  ut_assert_true(ut_object_implements_map(value8));
  ut_assert_int_equal(ut_map_get_length(value8), 1);
  UtObject *value8a = ut_map_lookup_string(value8, "name");
  ut_assert_true(ut_object_implements_string(value8a));
  ut_assert_cstring_equal(ut_string_get_text(value8a), "Arthur Dent");
  UtObject *value8b = ut_map_lookup_string(value8, "age");
  ut_assert_null_object(value8b);

  // Optional components (none present).
  UtObjectRef data9 = ut_uint8_list_new_from_hex_string("3100");
  UtObjectRef decoder9 = ut_asn1_ber_decoder_new(data9);
  UtObjectRef components9 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type9 = ut_asn1_set_type_new(components9, false);
  UtObjectRef value9 = ut_asn1_decoder_decode_value(decoder9, type9);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder9));
  ut_assert_true(ut_object_implements_map(value9));
  ut_assert_int_equal(ut_map_get_length(value9), 0);

  // Optional components (first present).
  UtObjectRef data10 =
      ut_uint8_list_new_from_hex_string("310d0c0b4172746875722044656e74");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  UtObjectRef components10 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type10 = ut_asn1_set_type_new(components10, false);
  UtObjectRef value10 = ut_asn1_decoder_decode_value(decoder10, type10);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder10));
  ut_assert_true(ut_object_implements_map(value10));
  ut_assert_int_equal(ut_map_get_length(value10), 1);
  UtObject *value10a = ut_map_lookup_string(value10, "name");
  ut_assert_true(ut_object_implements_string(value10a));
  ut_assert_cstring_equal(ut_string_get_text(value10a), "Arthur Dent");
  UtObject *value10b = ut_map_lookup_string(value10, "age");
  ut_assert_null_object(value10b);

  // Optional components (second present).
  UtObjectRef data11 = ut_uint8_list_new_from_hex_string("310302012a");
  UtObjectRef decoder11 = ut_asn1_ber_decoder_new(data11);
  UtObjectRef components11 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_optional_type_new_take(ut_asn1_utf8_string_type_new()),
      "age", ut_asn1_optional_type_new_take(ut_asn1_integer_type_new()), NULL);
  UtObjectRef type11 = ut_asn1_set_type_new(components11, false);
  UtObjectRef value11 = ut_asn1_decoder_decode_value(decoder11, type11);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder11));
  ut_assert_true(ut_object_implements_map(value11));
  ut_assert_int_equal(ut_map_get_length(value11), 1);
  UtObject *value11a = ut_map_lookup_string(value11, "name");
  ut_assert_null_object(value11a);
  UtObject *value11b = ut_map_lookup_string(value11, "age");
  ut_assert_true(ut_object_is_int64(value11b));
  ut_assert_int_equal(ut_int64_get_value(value11b), 42);

  // Default component (present).
  UtObjectRef data12 =
      ut_uint8_list_new_from_hex_string("31100c0b4172746875722044656e7402012a");
  UtObjectRef decoder12 = ut_asn1_ber_decoder_new(data12);
  UtObjectRef components12 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type12 = ut_asn1_set_type_new(components12, false);
  UtObjectRef value12 = ut_asn1_decoder_decode_value(decoder12, type12);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder12));
  ut_assert_true(ut_object_implements_map(value12));
  ut_assert_int_equal(ut_map_get_length(value12), 2);
  UtObject *value12a = ut_map_lookup_string(value12, "name");
  ut_assert_true(ut_object_implements_string(value12a));
  ut_assert_cstring_equal(ut_string_get_text(value12a), "Arthur Dent");
  UtObject *value12b = ut_map_lookup_string(value12, "age");
  ut_assert_true(ut_object_is_int64(value12b));
  ut_assert_int_equal(ut_int64_get_value(value12b), 42);

  // Default component (present with default value).
  UtObjectRef data13 =
      ut_uint8_list_new_from_hex_string("31100c0b4172746875722044656e74020163");
  UtObjectRef decoder13 = ut_asn1_ber_decoder_new(data13);
  UtObjectRef components13 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type13 = ut_asn1_set_type_new(components13, false);
  UtObjectRef value13 = ut_asn1_decoder_decode_value(decoder13, type13);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder13));
  ut_assert_true(ut_object_implements_map(value13));
  ut_assert_int_equal(ut_map_get_length(value13), 2);
  UtObject *value13a = ut_map_lookup_string(value13, "name");
  ut_assert_true(ut_object_implements_string(value13a));
  ut_assert_cstring_equal(ut_string_get_text(value13a), "Arthur Dent");
  UtObject *value13b = ut_map_lookup_string(value13, "age");
  ut_assert_true(ut_object_is_int64(value13b));
  ut_assert_int_equal(ut_int64_get_value(value13b), 99);

  // Default component (not present).
  UtObjectRef data14 =
      ut_uint8_list_new_from_hex_string("310d0c0b4172746875722044656e74");
  UtObjectRef decoder14 = ut_asn1_ber_decoder_new(data14);
  UtObjectRef components14 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age",
      ut_asn1_default_type_new_take(ut_asn1_integer_type_new(),
                                    ut_int64_new(99)),
      NULL);
  UtObjectRef type14 = ut_asn1_set_type_new(components14, false);
  UtObjectRef value14 = ut_asn1_decoder_decode_value(decoder14, type14);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder14));
  ut_assert_true(ut_object_implements_map(value14));
  ut_assert_int_equal(ut_map_get_length(value14), 2);
  UtObject *value14a = ut_map_lookup_string(value14, "name");
  ut_assert_true(ut_object_implements_string(value14a));
  ut_assert_cstring_equal(ut_string_get_text(value14a), "Arthur Dent");
  UtObject *value14b = ut_map_lookup_string(value14, "age");
  ut_assert_true(ut_object_is_int64(value14b));
  ut_assert_int_equal(ut_int64_get_value(value14b), 99);

  // Non-constructed form.
  UtObjectRef data20 = ut_uint8_list_new_from_hex_string("1100");
  UtObjectRef decoder20 = ut_asn1_ber_decoder_new(data20);
  UtObjectRef set20 = ut_asn1_ber_decoder_decode_set(decoder20);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder20),
                                      "Set must be constructed");

  // Missing component (age).
  UtObjectRef data21 =
      ut_uint8_list_new_from_hex_string("310d0c0b4172746875722044656e74");
  UtObjectRef decoder21 = ut_asn1_ber_decoder_new(data21);
  UtObjectRef components21 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type21 = ut_asn1_set_type_new(components21, false);
  UtObjectRef value21 = ut_asn1_decoder_decode_value(decoder21, type21);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder21),
                                      "Required SET components missing");

  // Additional component (not extensible).
  UtObjectRef data22 = ut_uint8_list_new_from_hex_string(
      "31130c0b4172746875722044656e7402012a0101ff");
  UtObjectRef decoder22 = ut_asn1_ber_decoder_new(data22);
  UtObjectRef components22 = ut_map_new_string_from_elements_take(
      "name", ut_asn1_utf8_string_type_new(), "age", ut_asn1_integer_type_new(),
      NULL);
  UtObjectRef type22 = ut_asn1_set_type_new(components22, false);
  UtObjectRef value22 = ut_asn1_decoder_decode_value(decoder22, type22);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder22),
                                      "Unknown SET component");
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
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_NUMERIC_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_numeric_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "12345 67890");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1200");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_numeric_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("320f120631323334352012053637383930");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_numeric_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "12345 67890");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("120b3132333435203637383930");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_numeric_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "12345 67890");

  // Invalid characters - "Example\tObject"
  UtObjectRef data10 =
      ut_uint8_list_new_from_hex_string("120b48656c6c6f20576f726c64");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  ut_cstring_ref string10 =
      ut_asn1_ber_decoder_decode_numeric_string(decoder10);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder10),
                                      "Invalid NumericString");
}

static void test_printable_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("130b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_PRINTABLE_STRING));
  ut_cstring_ref string1 =
      ut_asn1_ber_decoder_decode_printable_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "Hello World");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1300");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 =
      ut_asn1_ber_decoder_decode_printable_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("330f130648656c6c6f201305576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 =
      ut_asn1_ber_decoder_decode_printable_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("130b48656c6c6f20576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_printable_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");

  // Invalid characters - "#invalid"
  UtObjectRef data10 =
      ut_uint8_list_new_from_hex_string("130823696e76616c696423");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  ut_cstring_ref string10 =
      ut_asn1_ber_decoder_decode_printable_string(decoder10);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder10),
                                      "Invalid PrintableString");
}

static void test_ia5_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("160b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_IA5_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_ia5_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "Hello World");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1600");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_ia5_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("360f160648656c6c6f201605576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_ia5_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("160b48656c6c6f20576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_ia5_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");

  // Invalid characters "\xff".
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("1601ff");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_cstring_ref string5 = ut_asn1_ber_decoder_decode_ia5_string(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid IA5String");
}

static void test_utc_time() {
  // UTC date time - "230823205806Z".
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("170d3233303832343230353830365a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_UTC_TIME));
  UtObjectRef value1 = ut_asn1_ber_decoder_decode_utc_time(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_int_equal(ut_date_time_get_year(value1), 2023);
  ut_assert_int_equal(ut_date_time_get_month(value1), 8);
  ut_assert_int_equal(ut_date_time_get_day(value1), 24);
  ut_assert_int_equal(ut_date_time_get_hour(value1), 20);
  ut_assert_int_equal(ut_date_time_get_minutes(value1), 58);
  ut_assert_int_equal(ut_date_time_get_seconds(value1), 6);
  ut_assert_int_equal(ut_date_time_get_utc_offset_minutes(value1), 0);

  // With UTC offset - "230823205806+1200".
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string(
      "17113233303832343230353830362b31323030");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder2),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_UTC_TIME));
  UtObjectRef value2 = ut_asn1_ber_decoder_decode_utc_time(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_int_equal(ut_date_time_get_year(value2), 2023);
  ut_assert_int_equal(ut_date_time_get_month(value2), 8);
  ut_assert_int_equal(ut_date_time_get_day(value2), 24);
  ut_assert_int_equal(ut_date_time_get_hour(value2), 20);
  ut_assert_int_equal(ut_date_time_get_minutes(value2), 58);
  ut_assert_int_equal(ut_date_time_get_seconds(value2), 6);
  ut_assert_int_equal(ut_date_time_get_utc_offset_minutes(value2), 720);

  // Constructed form - "230823" "205806" "+1200".
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string(
      "37171706323330383234170632303538303617052b31323030");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef value3 = ut_asn1_ber_decoder_decode_utc_time(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_int_equal(ut_date_time_get_year(value3), 2023);
  ut_assert_int_equal(ut_date_time_get_month(value3), 8);
  ut_assert_int_equal(ut_date_time_get_day(value3), 24);
  ut_assert_int_equal(ut_date_time_get_hour(value3), 20);
  ut_assert_int_equal(ut_date_time_get_minutes(value3), 58);
  ut_assert_int_equal(ut_date_time_get_seconds(value3), 6);
  ut_assert_int_equal(ut_date_time_get_utc_offset_minutes(value3), 720);

  // Decoded as object with type.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string(
      "17113233303832343230353830362b31323030");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_utc_time_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_is_date_time(value4));
  ut_assert_int_equal(ut_date_time_get_year(value4), 2023);
  ut_assert_int_equal(ut_date_time_get_month(value4), 8);
  ut_assert_int_equal(ut_date_time_get_day(value4), 24);
  ut_assert_int_equal(ut_date_time_get_hour(value4), 20);
  ut_assert_int_equal(ut_date_time_get_minutes(value4), 58);
  ut_assert_int_equal(ut_date_time_get_seconds(value4), 6);
  ut_assert_int_equal(ut_date_time_get_utc_offset_minutes(value4), 720);

  // Empty string not valid.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("1700");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  UtObjectRef value5 = ut_asn1_ber_decoder_decode_utc_time(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid UTCTime");

  // Just date, missing time - "230823".
  UtObjectRef data6 = ut_uint8_list_new_from_hex_string("1706323330383234");
  UtObjectRef decoder6 = ut_asn1_ber_decoder_new(data6);
  UtObjectRef value6 = ut_asn1_ber_decoder_decode_utc_time(decoder6);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder6),
                                      "Invalid UTCTime");

  // Date and time, missing UTC offset - "230823205806".
  UtObjectRef data7 =
      ut_uint8_list_new_from_hex_string("170c32333038323432303538303617");
  UtObjectRef decoder7 = ut_asn1_ber_decoder_new(data7);
  UtObjectRef value7 = ut_asn1_ber_decoder_decode_utc_time(decoder7);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder7),
                                      "Invalid UTCTime");

  // Missing leading zeros - "2382320586Z".
  UtObjectRef data8 =
      ut_uint8_list_new_from_hex_string("170b323338323432303538365a");
  UtObjectRef decoder8 = ut_asn1_ber_decoder_new(data8);
  UtObjectRef value8 = ut_asn1_ber_decoder_decode_utc_time(decoder8);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder8),
                                      "Invalid UTCTime");

  // Four digit year - "20230823205806Z".
  UtObjectRef data9 =
      ut_uint8_list_new_from_hex_string("170f32303233303832343230353830365a");
  UtObjectRef decoder9 = ut_asn1_ber_decoder_new(data9);
  UtObjectRef value9 = ut_asn1_ber_decoder_decode_utc_time(decoder9);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder9),
                                      "Invalid UTCTime");

  // Invalid character in date "2b0823205806Z".
  UtObjectRef data10 =
      ut_uint8_list_new_from_hex_string("170d3262303832343230353830365a");
  UtObjectRef decoder10 = ut_asn1_ber_decoder_new(data10);
  UtObjectRef value10 = ut_asn1_ber_decoder_decode_utc_time(decoder10);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder10),
                                      "Invalid UTCTime");
}

static void test_graphic_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("190b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_GRAPHIC_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_graphic_string(decoder1);
  ut_assert_cstring_equal(string1, "Hello World");
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1900");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_graphic_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("390f190648656c6c6f201905576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_graphic_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("190b48656c6c6f20576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_graphic_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");

  // Invalid characters - "Hello\tWorld".
  UtObjectRef data5 =
      ut_uint8_list_new_from_hex_string("190b48656c6c6f09576f726c64");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_cstring_ref string5 = ut_asn1_ber_decoder_decode_graphic_string(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid GraphicString");
}

static void test_visible_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("1a0b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_VISIBLE_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_visible_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "Hello World");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1a00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_visible_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("3a0f1a0648656c6c6f201a05576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_visible_string(decoder3);
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("1a0b48656c6c6f20576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_visible_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");

  // Invalid characters "\x01".
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string("1a0101");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_cstring_ref string5 = ut_asn1_ber_decoder_decode_visible_string(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid VisibleString");
}

static void test_general_string() {
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("1b0b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_GENERAL_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_general_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "Hello World");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1b00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_general_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 =
      ut_uint8_list_new_from_hex_string("3b0f1b0648656c6c6f201b05576f726c64");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_general_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 =
      ut_uint8_list_new_from_hex_string("1b0b48656c6c6f20576f726c64");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_general_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");
}

static void test_bmp_string() {
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string(
      "1e1600480065006c006c006f00200057006f0072006c0064");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(decoder1),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_BMP_STRING));
  ut_cstring_ref string1 = ut_asn1_ber_decoder_decode_bmp_string(decoder1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_cstring_equal(string1, "Hello World");

  // Empty string.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("1e00");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  ut_cstring_ref string2 = ut_asn1_ber_decoder_decode_bmp_string(decoder2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_cstring_equal(string2, "");

  // Constructed form.
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string(
      "3e1a1e0c00480065006c006c006f00201e0a0057006f0072006c0064");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  ut_cstring_ref string3 = ut_asn1_ber_decoder_decode_bmp_string(decoder3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_cstring_equal(string3, "Hello World");

  // Decoded as object with type.
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string(
      "1e1600480065006c006c006f00200057006f0072006c0064");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_bmp_string_type_new();
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_implements_string(value4));
  ut_assert_cstring_equal(ut_string_get_text(value4), "Hello World");

  // Containing half a 16 bit value.
  UtObjectRef data5 = ut_uint8_list_new_from_hex_string(
      "1e1500480065006c006c006f00200057006f0072006c64");
  UtObjectRef decoder5 = ut_asn1_ber_decoder_new(data5);
  ut_cstring_ref string5 = ut_asn1_ber_decoder_decode_bmp_string(decoder5);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder5),
                                      "Invalid BMPString");
}

static void test_choice() {
  // First choice.
  UtObjectRef data1 =
      ut_uint8_list_new_from_hex_string("0c0b48656c6c6f20576f726c64");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  UtObjectRef components1 = ut_map_new_string_from_elements_take(
      "text", ut_asn1_utf8_string_type_new(), "number",
      ut_asn1_integer_type_new(), NULL);
  UtObjectRef type1 = ut_asn1_choice_type_new(components1, false);
  UtObjectRef value1 = ut_asn1_decoder_decode_value(decoder1, type1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_true(ut_object_is_asn1_choice_value(value1));
  ut_assert_cstring_equal(ut_asn1_choice_value_get_identifier(value1), "text");
  UtObject *value1i = ut_asn1_choice_value_get_value(value1);
  ut_assert_true(ut_object_implements_string(value1i));
  ut_assert_cstring_equal(ut_string_get_text(value1i), "Hello World");

  // Second choice.
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("02012a");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef components2 = ut_map_new_string_from_elements_take(
      "text", ut_asn1_utf8_string_type_new(), "number",
      ut_asn1_integer_type_new(), NULL);
  UtObjectRef type2 = ut_asn1_choice_type_new(components2, false);
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_asn1_choice_value(value2));
  ut_assert_cstring_equal(ut_asn1_choice_value_get_identifier(value2),
                          "number");
  UtObject *value2i = ut_asn1_choice_value_get_value(value2);
  ut_assert_true(ut_object_is_int64(value2i));
  ut_assert_int_equal(ut_int64_get_value(value2i), 42);

  // Unknown value (extensible).
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("0101ff");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef components3 = ut_map_new_string_from_elements_take(
      "text", ut_asn1_utf8_string_type_new(), "number",
      ut_asn1_integer_type_new(), NULL);
  UtObjectRef type3 = ut_asn1_choice_type_new(components3, true);
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_is_asn1_choice_value(value3));
  ut_assert_cstring_equal(ut_asn1_choice_value_get_identifier(value3), "");
  UtObject *value3i = ut_asn1_choice_value_get_value(value3);
  ut_assert_true(ut_object_is_asn1_ber_decoder(value3i));
  ut_assert_true(ut_asn1_tag_matches(ut_asn1_ber_decoder_get_tag(value3i),
                                     UT_ASN1_TAG_CLASS_UNIVERSAL,
                                     UT_ASN1_TAG_UNIVERSAL_BOOLEAN));
  ut_assert_uint8_list_equal_hex(ut_asn1_ber_decoder_get_contents(value3i),
                                 "ff");

  // Unknown value (not extensible).
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("0101ff");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef components4 = ut_map_new_string_from_elements_take(
      "text", ut_asn1_utf8_string_type_new(), "number",
      ut_asn1_integer_type_new(), NULL);
  UtObjectRef type4 = ut_asn1_choice_type_new(components4, false);
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_is_error_with_description(ut_asn1_decoder_get_error(decoder4),
                                      "Unknown CHOICE value");
}

static void test_tagged_type() {
  // Explicit tag ([1] INTEGER).
  UtObjectRef data1 = ut_uint8_list_new_from_hex_string("a10302012a");
  UtObjectRef decoder1 = ut_asn1_ber_decoder_new(data1);
  UtObjectRef type1 = ut_asn1_tagged_type_new_take(
      UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, true, ut_asn1_integer_type_new());
  UtObjectRef value1 = ut_asn1_decoder_decode_value(decoder1, type1);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder1));
  ut_assert_true(ut_object_is_int64(value1));
  ut_assert_int_equal(ut_int64_get_value(value1), 42);

  // Implicit tag ([1] IMPLICIT INTEGER).
  UtObjectRef data2 = ut_uint8_list_new_from_hex_string("81012a");
  UtObjectRef decoder2 = ut_asn1_ber_decoder_new(data2);
  UtObjectRef type2 = ut_asn1_tagged_type_new_take(
      UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, false, ut_asn1_integer_type_new());
  UtObjectRef value2 = ut_asn1_decoder_decode_value(decoder2, type2);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder2));
  ut_assert_true(ut_object_is_int64(value2));
  ut_assert_int_equal(ut_int64_get_value(value2), 42);

  // Nested explicit tags ([1] [2] INTEGER).
  UtObjectRef data3 = ut_uint8_list_new_from_hex_string("a105a20302012a");
  UtObjectRef decoder3 = ut_asn1_ber_decoder_new(data3);
  UtObjectRef type3 = ut_asn1_tagged_type_new_take(
      UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, true,
      ut_asn1_tagged_type_new_take(UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 2, true,
                                   ut_asn1_integer_type_new()));
  UtObjectRef value3 = ut_asn1_decoder_decode_value(decoder3, type3);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder3));
  ut_assert_true(ut_object_is_int64(value3));
  ut_assert_int_equal(ut_int64_get_value(value3), 42);

  // Nested implicit tags ([1] IMPLICIT [2] IMPLICIT INTEGER).
  UtObjectRef data4 = ut_uint8_list_new_from_hex_string("81012a");
  UtObjectRef decoder4 = ut_asn1_ber_decoder_new(data4);
  UtObjectRef type4 = ut_asn1_tagged_type_new_take(
      UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 1, false,
      ut_asn1_tagged_type_new_take(UT_ASN1_TAG_CLASS_CONTEXT_SPECIFIC, 2, false,
                                   ut_asn1_integer_type_new()));
  UtObjectRef value4 = ut_asn1_decoder_decode_value(decoder4, type4);
  ut_assert_null_object(ut_asn1_decoder_get_error(decoder4));
  ut_assert_true(ut_object_is_int64(value4));
  ut_assert_int_equal(ut_int64_get_value(value4), 42);
}

int main(int argc, char **argv) {
  test_boolean();
  test_integer();
  test_bit_string();
  test_octet_string();
  test_null();
  test_object_identifier();
  test_object_descriptor();
  test_external();
  test_real();
  test_enumerated();
  test_embedded_pdv();
  test_utf8_string();
  test_relative_oid();
  test_sequence();
  test_sequence_of();
  test_set();
  test_set_of();
  test_numeric_string();
  test_printable_string();
  test_ia5_string();
  test_utc_time();
  test_graphic_string();
  test_visible_string();
  test_general_string();
  test_bmp_string();
  test_choice();
  test_tagged_type();

  return 0;
}
