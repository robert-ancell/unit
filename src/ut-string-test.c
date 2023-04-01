#include "ut.h"

static void test_utf8() {
  UtObjectRef string = ut_string_new("$¢€𐐷😀");

  UtObjectRef utf8 = ut_string_get_utf8(string);
  ut_assert_int_equal(ut_list_get_length(utf8), 14);
  // $
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 0), 0x24);
  // ¢
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 1), 0xc2);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 2), 0xa2);
  // €
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 3), 0xe2);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 4), 0x82);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 5), 0xac);
  // 𐐷
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 6), 0xf0);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 7), 0x90);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 8), 0x90);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 9), 0xb7);
  // 😀
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 10), 0xf0);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 11), 0x9f);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 12), 0x98);
  ut_assert_int_equal(ut_uint8_list_get_element(utf8, 13), 0x80);
}

static void test_utf16() {
  UtObjectRef string = ut_string_new("$¢€𐐷😀");

  UtObjectRef utf16 = ut_string_get_utf16(string);
  ut_assert_int_equal(ut_list_get_length(utf16), 7);
  // $
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 0), 0x24);
  // ¢
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 1), 0xa2);
  // €
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 2), 0x20ac);
  // 𐐷
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 3), 0xd801);
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 4), 0xdc37);
  // 😀
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 5), 0xd83d);
  ut_assert_int_equal(ut_uint16_list_get_element(utf16, 6), 0xde00);

  UtObjectRef string2 = ut_string_new_from_utf16(utf16);
  ut_assert_cstring_equal(ut_string_get_text(string2), "$¢€𐐷😀");
}

static void test_iso8859_1() {
  UtObjectRef string = ut_string_new("Ün¡+");

  UtObjectRef characters = ut_string_get_iso_8859_1(string);
  ut_assert_int_equal(ut_list_get_length(characters), 4);
  // Ü
  ut_assert_int_equal(ut_uint8_list_get_element(characters, 0), 0xdc);
  // n
  ut_assert_int_equal(ut_uint8_list_get_element(characters, 1), 0x6e);
  // ¡
  ut_assert_int_equal(ut_uint8_list_get_element(characters, 2), 0xa1);
  // +
  ut_assert_int_equal(ut_uint8_list_get_element(characters, 3), 0x2b);

  UtObjectRef string2 = ut_string_new_from_iso_8859_1(characters);
  ut_assert_cstring_equal(ut_string_get_text(string2), "Ün¡+");
}

int main(int argc, char **argv) {
  UtObjectRef empty = ut_string_new("");
  ut_assert_cstring_equal(ut_string_get_text(empty), "");

  UtObjectRef emoji = ut_string_new("😀");
  ut_assert_cstring_equal(ut_string_get_text(emoji), "😀");

  UtObjectRef string = ut_string_new("Hello World!");
  ut_assert_cstring_equal(ut_string_get_text(string), "Hello World!");

  UtObjectRef cleared = ut_string_new("Hello World!");
  ut_string_clear(cleared);
  ut_assert_cstring_equal(ut_string_get_text(cleared), "");

  UtObjectRef prepended1 = ut_string_new("");
  ut_string_prepend(prepended1, "Hello World!");
  ut_assert_cstring_equal(ut_string_get_text(prepended1), "Hello World!");

  UtObjectRef prepended2 = ut_string_new("World!");
  ut_string_prepend(prepended2, "Hello ");
  ut_assert_cstring_equal(ut_string_get_text(prepended2), "Hello World!");

  UtObjectRef prepended3 = ut_string_new("");
  ut_string_prepend_code_point(prepended3, 'A');
  ut_assert_cstring_equal(ut_string_get_text(prepended3), "A");

  UtObjectRef prepended4 = ut_string_new("BC");
  ut_string_prepend_code_point(prepended4, 'A');
  ut_assert_cstring_equal(ut_string_get_text(prepended4), "ABC");

  UtObjectRef prepended5 = ut_string_new("");
  ut_string_prepend_code_point(prepended5, 0x1f600);
  ut_assert_cstring_equal(ut_string_get_text(prepended5), "😀");

  UtObjectRef prepended6 = ut_string_new(" Smile!");
  ut_string_prepend_code_point(prepended6, 0x1f600);
  ut_assert_cstring_equal(ut_string_get_text(prepended6), "😀 Smile!");

  UtObjectRef appended1 = ut_string_new("");
  ut_string_append(appended1, "Hello World!");
  ut_assert_cstring_equal(ut_string_get_text(appended1), "Hello World!");

  UtObjectRef appended2 = ut_string_new("Hello");
  ut_string_append(appended2, " World!");
  ut_assert_cstring_equal(ut_string_get_text(appended2), "Hello World!");

  UtObjectRef appended3 = ut_string_new("");
  ut_string_append_code_point(appended3, 'C');
  ut_assert_cstring_equal(ut_string_get_text(appended3), "C");

  UtObjectRef appended4 = ut_string_new("AB");
  ut_string_append_code_point(appended4, 'C');
  ut_assert_cstring_equal(ut_string_get_text(appended4), "ABC");

  UtObjectRef appended5 = ut_string_new("");
  ut_string_append_code_point(appended5, 0x1f600);
  ut_assert_cstring_equal(ut_string_get_text(appended5), "😀");

  UtObjectRef appended6 = ut_string_new("Smile! ");
  ut_string_append_code_point(appended6, 0x1f600);
  ut_assert_cstring_equal(ut_string_get_text(appended6), "Smile! 😀");

  UtObjectRef encoded = ut_string_new("$¢€𐐷😀");
  UtObjectRef code_points = ut_string_get_code_points(encoded);
  ut_assert_int_equal(ut_list_get_length(code_points), 5);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 0), 0x24);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 1), 0xa2);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 2), 0x20ac);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 3), 0x10437);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 4), 0x1f600);

  test_utf8();
  test_utf16();
  test_iso8859_1();

  return 0;
}
