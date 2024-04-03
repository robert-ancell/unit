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

  ut_cstring_ref join1_value =
      ut_cstring_new_join(",", "one", "two", "three", NULL);
  ut_assert_cstring_equal(join1_value, "one,two,three");

  ut_cstring_ref join2_value = ut_cstring_new_join(",", "one", NULL);
  ut_assert_cstring_equal(join2_value, "one");

  ut_cstring_ref join3_value = ut_cstring_new_join(",", NULL);
  ut_assert_cstring_equal(join3_value, "");

  ut_cstring_ref join4_value =
      ut_cstring_new_join("--", "one", "two", "three", NULL);
  ut_assert_cstring_equal(join4_value, "one--two--three");

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

  UtObjectRef values1 = ut_cstring_split("", "");
  ut_assert_int_equal(ut_list_get_length(values1), 0);

  UtObjectRef values2 = ut_cstring_split("", ",");
  ut_assert_int_equal(ut_list_get_length(values2), 0);

  UtObjectRef values3 = ut_cstring_split("one", ",");
  ut_assert_int_equal(ut_list_get_length(values3), 1);
  ut_assert_cstring_equal(ut_string_list_get_element(values3, 0), "one");

  UtObjectRef values4 = ut_cstring_split("one,two", ",");
  ut_assert_int_equal(ut_list_get_length(values4), 2);
  ut_assert_cstring_equal(ut_string_list_get_element(values4, 0), "one");
  ut_assert_cstring_equal(ut_string_list_get_element(values4, 1), "two");

  UtObjectRef values5 = ut_cstring_split("one,two,three", ",");
  ut_assert_int_equal(ut_list_get_length(values5), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values5, 0), "one");
  ut_assert_cstring_equal(ut_string_list_get_element(values5, 1), "two");
  ut_assert_cstring_equal(ut_string_list_get_element(values5, 2), "three");

  UtObjectRef values6 = ut_cstring_split(",two,three", ",");
  ut_assert_int_equal(ut_list_get_length(values6), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values6, 0), "");
  ut_assert_cstring_equal(ut_string_list_get_element(values6, 1), "two");
  ut_assert_cstring_equal(ut_string_list_get_element(values6, 2), "three");

  UtObjectRef values7 = ut_cstring_split("one,two,", ",");
  ut_assert_int_equal(ut_list_get_length(values7), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values7, 0), "one");
  ut_assert_cstring_equal(ut_string_list_get_element(values7, 1), "two");
  ut_assert_cstring_equal(ut_string_list_get_element(values7, 2), "");

  UtObjectRef values8 = ut_cstring_split(",,", ",");
  ut_assert_int_equal(ut_list_get_length(values8), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values8, 0), "");
  ut_assert_cstring_equal(ut_string_list_get_element(values8, 1), "");
  ut_assert_cstring_equal(ut_string_list_get_element(values8, 2), "");

  UtObjectRef values9 = ut_cstring_split("one--two--three", "--");
  ut_assert_int_equal(ut_list_get_length(values9), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values9, 0), "one");
  ut_assert_cstring_equal(ut_string_list_get_element(values9, 1), "two");
  ut_assert_cstring_equal(ut_string_list_get_element(values9, 2), "three");

  UtObjectRef values10 = ut_cstring_split("----", "--");
  ut_assert_int_equal(ut_list_get_length(values10), 3);
  ut_assert_cstring_equal(ut_string_list_get_element(values10, 0), "");
  ut_assert_cstring_equal(ut_string_list_get_element(values10, 1), "");
  ut_assert_cstring_equal(ut_string_list_get_element(values10, 2), "");

  return 0;
}
