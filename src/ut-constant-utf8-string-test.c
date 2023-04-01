#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef string = ut_constant_utf8_string_new("Hello");
  UtObjectRef code_points = ut_string_get_code_points(string);
  ut_assert_int_equal(ut_list_get_length(code_points), 5);
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 0), 'H');
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 1), 'e');
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 2), 'l');
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 3), 'l');
  ut_assert_int_equal(ut_uint32_list_get_element(code_points, 4), 'o');

  return 0;
}
