#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

static void test_encode() {
  UtObjectRef empty_list = ut_uint8_array_new();
  ut_cstring_ref empty_text = ut_base64_encode(empty_list);
  ut_assert_cstring_equal(empty_text, "");

  UtObjectRef short1_list = ut_uint8_list_new_from_elements(1, 'M');
  ut_cstring_ref short1_text = ut_base64_encode(short1_list);
  ut_assert_cstring_equal(short1_text, "TQ==");

  UtObjectRef short2_list = ut_uint8_list_new_from_elements(2, 'M', 'a');
  ut_cstring_ref short2_text = ut_base64_encode(short2_list);
  ut_assert_cstring_equal(short2_text, "TWE=");

  UtObjectRef short3_list = ut_uint8_list_new_from_elements(3, 'M', 'a', 'n');
  ut_cstring_ref short3_text = ut_base64_encode(short3_list);
  ut_assert_cstring_equal(short3_text, "TWFu");

  UtObjectRef sentence = ut_string_new("Many hands make light work.");
  UtObjectRef sentence_list = ut_string_get_utf8(sentence);
  ut_cstring_ref sentence_text = ut_base64_encode(sentence_list);
  ut_assert_cstring_equal(sentence_text,
                          "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");

  UtObjectRef binary_list = ut_uint8_list_new();
  for (size_t i = 0; i < 256; i++) {
    ut_uint8_list_append(binary_list, i);
  }
  ut_cstring_ref binary_text = ut_base64_encode(binary_list);
  ut_assert_cstring_equal(
      binary_text,
      "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vM"
      "DEyMzQ1Njc4OTo7PD0+"
      "P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub"
      "3BxcnN0dXZ3eHl6e3x9fn+"
      "AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+"
      "wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/"
      "g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==");
}

static void test_decode() {
  UtObjectRef empty = ut_base64_decode("");
  ut_assert_non_null(empty);
  ut_assert_true(ut_object_implements_uint8_list(empty));
  ut_assert_int_equal(ut_list_get_length(empty), 0);

  UtObjectRef whitespace = ut_base64_decode(" ");
  ut_assert_non_null(whitespace);
  ut_assert_is_error_with_description(whitespace, "Invalid Base64");

  UtObjectRef unknown_character = ut_base64_decode("!");
  ut_assert_non_null(unknown_character);
  ut_assert_is_error_with_description(unknown_character, "Invalid Base64");

  UtObjectRef non_ascii_character = ut_base64_decode("\xff");
  ut_assert_non_null(non_ascii_character);
  ut_assert_is_error_with_description(non_ascii_character, "Invalid Base64");

  UtObjectRef missing_data = ut_base64_decode("T");
  ut_assert_non_null(missing_data);
  ut_assert_is_error_with_description(missing_data, "Invalid Base64");

  UtObjectRef short1a = ut_base64_decode("TQ==");
  ut_assert_non_null(short1a);
  ut_assert_true(ut_object_implements_uint8_list(short1a));
  ut_assert_int_equal(ut_list_get_length(short1a), 1);

  UtObjectRef short1b = ut_base64_decode("TQ");
  ut_assert_non_null(short1b);
  ut_assert_true(ut_object_implements_uint8_list(short1b));
  ut_assert_int_equal(ut_list_get_length(short1b), 1);

  UtObjectRef short2a = ut_base64_decode("TWE=");
  ut_assert_non_null(short2a);
  ut_assert_true(ut_object_implements_uint8_list(short2a));
  ut_assert_int_equal(ut_list_get_length(short2a), 2);

  UtObjectRef short2b = ut_base64_decode("TWE");
  ut_assert_non_null(short2b);
  ut_assert_true(ut_object_implements_uint8_list(short2b));
  ut_assert_int_equal(ut_list_get_length(short2b), 2);

  UtObjectRef short3 = ut_base64_decode("TQFu");
  ut_assert_non_null(short3);
  ut_assert_true(ut_object_implements_uint8_list(short3));
  ut_assert_int_equal(ut_list_get_length(short3), 3);

  UtObjectRef sentence =
      ut_base64_decode("TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");
  ut_assert_non_null(sentence);
  ut_assert_true(ut_object_implements_uint8_list(sentence));
  UtObjectRef sentence_text = ut_utf8_string_new_from_data(sentence);
  ut_assert_cstring_equal(ut_string_get_text(sentence_text),
                          "Many hands make light work.");

  UtObjectRef binary = ut_base64_decode(
      "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4vMDEyMzQ1"
      "Njc4OTo7PD0+"
      "P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5fYGFiY2RlZmdoaWprbG1ub3BxcnN0"
      "dXZ3eHl6e3x9fn+"
      "AgYKDhIWGh4iJiouMjY6PkJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+"
      "wsbKztLW2t7i5uru8vb6/wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/"
      "g4eLj5OXm5+jp6uvs7e7v8PHy8/T19vf4+fr7/P3+/w==");
  ut_assert_non_null(binary);
  ut_assert_true(ut_object_implements_uint8_list(binary));
  ut_assert_int_equal(ut_list_get_length(binary), 256);
  for (size_t i = 0; i < 256; i++) {
    ut_assert_int_equal(ut_uint8_list_get_element(binary, i), i);
  }
}

int main(int argc, char **argv) {
  test_encode();
  test_decode();
}
