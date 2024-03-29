#include "ut.h"

int main(int argc, char **argv) {
  ut_cstring_ref new_value = ut_cstring_new("Hello World");
  ut_assert_cstring_equal(new_value, "Hello World");

  ut_cstring_ref substring1_value = ut_cstring_new_substring("World", 0, 5);
  ut_assert_cstring_equal(substring1_value, "World");

  ut_cstring_ref substring2_value = ut_cstring_new_substring("World", 1, 5);
  ut_assert_cstring_equal(substring2_value, "orld");

  ut_cstring_ref substring3_value = ut_cstring_new_substring("World", 0, 4);
  ut_assert_cstring_equal(substring3_value, "Worl");

  ut_cstring_ref substring4_value = ut_cstring_new_substring("World", 1, 4);
  ut_assert_cstring_equal(substring4_value, "orl");

  ut_cstring_ref sized_value = ut_cstring_new_sized("World", 3);
  ut_assert_cstring_equal(sized_value, "Wor");

  ut_cstring_ref lower_value = ut_cstring_new_lowercase("HeLlO");
  ut_assert_cstring_equal(lower_value, "hello");

  ut_cstring_ref upper_value = ut_cstring_new_uppercase("HeLlO");
  ut_assert_cstring_equal(upper_value, "HELLO");

  ut_cstring_ref from_words1_value =
      ut_cstring_new_from_words(",", "one", "two", "three", NULL);
  ut_assert_cstring_equal(from_words1_value, "one,two,three");

  ut_cstring_ref from_words2_value =
      ut_cstring_new_from_words(",", "one", NULL);
  ut_assert_cstring_equal(from_words2_value, "one");

  ut_cstring_ref from_words3_value = ut_cstring_new_from_words(",", NULL);
  ut_assert_cstring_equal(from_words3_value, "");

  ut_cstring_ref printf_value = ut_cstring_new_printf("Number %d", 1);
  ut_assert_cstring_equal(printf_value, "Number 1");

  ut_cstring_ref prefix_value = ut_cstring_new("Food");
  ut_assert_true(ut_cstring_starts_with(prefix_value, "Foo"));
  ut_assert_false(ut_cstring_starts_with(prefix_value, "Bar"));
  ut_assert_false(ut_cstring_starts_with(prefix_value, "For"));

  ut_cstring_ref suffix_value = ut_cstring_new("Phone");
  ut_assert_true(ut_cstring_ends_with(suffix_value, "one"));
  ut_assert_false(ut_cstring_ends_with(suffix_value, "two"));
  ut_assert_false(ut_cstring_ends_with(suffix_value, "ine"));

  return 0;
}
