#include <stdlib.h>

#include "ut.h"

static void test_append_uint16() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_uint16_le(array1, 0x1234);
  ut_assert_uint8_list_equal_hex(array1, "3412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_uint16_be(array2, 0x1234);
  ut_assert_uint8_list_equal_hex(array2, "1234");
}

static void test_get_uint16() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("3412");
  ut_assert_int_equal(ut_uint8_list_get_uint16_le(array1, 0), 0x1234);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("1234");
  ut_assert_int_equal(ut_uint8_list_get_uint16_be(array2, 0), 0x1234);
}

static void test_append_int16() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_int16_le(array1, 0x1234);
  ut_assert_uint8_list_equal_hex(array1, "3412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_int16_le(array2, -0x1234);
  ut_assert_uint8_list_equal_hex(array2, "cced");

  UtObjectRef array3 = ut_uint8_array_new();
  ut_uint8_list_append_int16_be(array3, 0x1234);
  ut_assert_uint8_list_equal_hex(array3, "1234");

  UtObjectRef array4 = ut_uint8_array_new();
  ut_uint8_list_append_int16_be(array4, -0x1234);
  ut_assert_uint8_list_equal_hex(array4, "edcc");
}

static void test_get_int16() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("3412");
  ut_assert_int_equal(ut_uint8_list_get_int16_le(array1, 0), 0x1234);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("cced");
  ut_assert_int_equal(ut_uint8_list_get_int16_le(array2, 0), -0x1234);

  UtObjectRef array3 = ut_uint8_array_new_from_hex_string("1234");
  ut_assert_int_equal(ut_uint8_list_get_int16_be(array3, 0), 0x1234);

  UtObjectRef array4 = ut_uint8_array_new_from_hex_string("edcc");
  ut_assert_int_equal(ut_uint8_list_get_int16_be(array4, 0), -0x1234);
}

static void test_append_uint32() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_uint32_le(array1, 0x12345678);
  ut_assert_uint8_list_equal_hex(array1, "78563412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_uint32_be(array2, 0x12345678);
  ut_assert_uint8_list_equal_hex(array2, "12345678");
}

static void test_get_uint32() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("78563412");
  ut_assert_int_equal(ut_uint8_list_get_uint32_le(array1, 0), 0x12345678);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("12345678");
  ut_assert_int_equal(ut_uint8_list_get_uint32_be(array2, 0), 0x12345678);
}

static void test_append_int32() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_int32_le(array1, 0x12345678);
  ut_assert_uint8_list_equal_hex(array1, "78563412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_int32_le(array2, -0x12345678);
  ut_assert_uint8_list_equal_hex(array2, "88a9cbed");

  UtObjectRef array3 = ut_uint8_array_new();
  ut_uint8_list_append_int32_be(array3, 0x12345678);
  ut_assert_uint8_list_equal_hex(array3, "12345678");

  UtObjectRef array4 = ut_uint8_array_new();
  ut_uint8_list_append_int32_be(array4, -0x12345678);
  ut_assert_uint8_list_equal_hex(array4, "edcba988");
}

static void test_get_int32() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("78563412");
  ut_assert_int_equal(ut_uint8_list_get_int32_le(array1, 0), 0x12345678);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("88a9cbed");
  ut_assert_int_equal(ut_uint8_list_get_int32_le(array2, 0), -0x12345678);

  UtObjectRef array3 = ut_uint8_array_new_from_hex_string("12345678");
  ut_assert_int_equal(ut_uint8_list_get_int32_be(array3, 0), 0x12345678);

  UtObjectRef array4 = ut_uint8_array_new_from_hex_string("edcba988");
  ut_assert_int_equal(ut_uint8_list_get_int32_be(array4, 0), -0x12345678);
}

static void test_append_uint64() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_uint64_le(array1, 0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array1, "f0debc9a78563412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_uint64_be(array2, 0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array2, "123456789abcdef0");
}

static void test_get_uint64() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("f0debc9a78563412");
  ut_assert_int_equal(ut_uint8_list_get_uint64_le(array1, 0),
                      0x123456789abcdef0);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("123456789abcdef0");
  ut_assert_int_equal(ut_uint8_list_get_uint64_be(array2, 0),
                      0x123456789abcdef0);
}

static void test_append_int64() {
  UtObjectRef array1 = ut_uint8_array_new();
  ut_uint8_list_append_int64_le(array1, 0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array1, "f0debc9a78563412");

  UtObjectRef array2 = ut_uint8_array_new();
  ut_uint8_list_append_int64_le(array2, -0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array2, "1021436587a9cbed");

  UtObjectRef array3 = ut_uint8_array_new();
  ut_uint8_list_append_int64_be(array3, 0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array3, "123456789abcdef0");

  UtObjectRef array4 = ut_uint8_array_new();
  ut_uint8_list_append_int64_be(array4, -0x123456789abcdef0);
  ut_assert_uint8_list_equal_hex(array4, "edcba98765432110");
}

static void test_get_int64() {
  UtObjectRef array1 = ut_uint8_array_new_from_hex_string("f0debc9a78563412");
  ut_assert_int_equal(ut_uint8_list_get_int64_le(array1, 0),
                      0x123456789abcdef0);

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("1021436587a9cbed");
  ut_assert_int_equal(ut_uint8_list_get_int64_le(array2, 0),
                      -0x123456789abcdef0);

  UtObjectRef array3 = ut_uint8_array_new_from_hex_string("123456789abcdef0");
  ut_assert_int_equal(ut_uint8_list_get_int64_be(array3, 0),
                      0x123456789abcdef0);

  UtObjectRef array4 = ut_uint8_array_new_from_hex_string("edcba98765432110");
  ut_assert_int_equal(ut_uint8_list_get_int64_be(array4, 0),
                      -0x123456789abcdef0);
}

int main(int argc, char **argv) {
  UtObjectRef array0 = ut_uint8_array_new();
  ut_assert_uint8_list_equal_hex(array0, "");

  UtObjectRef array1 = ut_uint8_array_new_from_elements(3, 0x00, 0x78, 0xff);
  ut_assert_uint8_list_equal_hex(array1, "0078ff");

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("0078ff");
  ut_assert_uint8_list_equal_hex(array2, "0078ff");
  ut_cstring_ref hex_string = ut_uint8_list_to_hex_string(array2);
  ut_assert_cstring_equal(hex_string, "0078ff");

  UtObjectRef array3 = ut_uint8_array_new_from_hex_string("0078FF");
  ut_assert_uint8_list_equal_hex(array3, "0078ff");

  UtObjectRef array4 = ut_uint8_array_new_from_hex_string("0078F");
  ut_assert_is_error_with_description(array4,
                                      "Invalid hexadecimal string length");

  UtObjectRef array5 = ut_uint8_array_new_from_hex_string("0078FG");
  ut_assert_is_error_with_description(array5, "Invalid hexadecimal string");

  UtObjectRef array6 = ut_uint8_array_new();
  ut_uint8_list_append(array6, 0x00);
  ut_uint8_list_append(array6, 0x78);
  ut_uint8_list_append(array6, 0xff);
  ut_assert_uint8_list_equal_hex(array6, "0078ff");

  UtObjectRef array7 = ut_uint8_array_new();
  ut_uint8_list_prepend(array7, 0xff);
  ut_uint8_list_prepend(array7, 0x78);
  ut_uint8_list_prepend(array7, 0x00);
  ut_assert_uint8_list_equal_hex(array7, "0078ff");

  UtObjectRef array8 = ut_uint8_array_new();
  ut_uint8_list_append(array8, 0x00);
  ut_uint8_list_append(array8, 0xff);
  ut_uint8_list_insert(array8, 1, 0x78);
  ut_assert_uint8_list_equal_hex(array8, "0078ff");

  test_append_uint16();
  test_get_uint16();

  test_append_int16();
  test_get_int16();

  test_append_uint32();
  test_get_uint32();

  test_append_int32();
  test_get_int32();

  test_append_uint64();
  test_get_uint64();

  test_append_int64();
  test_get_int64();

  return 0;
}
