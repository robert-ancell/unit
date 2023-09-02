#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef array1 = ut_boolean_array_new();
  ut_assert_int_equal(ut_list_get_length(array1), 0);

  UtObjectRef array2 = ut_boolean_array_new();
  ut_boolean_list_append(array2, false);
  ut_boolean_list_append(array2, true);
  ut_boolean_list_append(array2, true);
  ut_assert_int_equal(ut_list_get_length(array2), 3);
  ut_assert_false(ut_boolean_list_get_element(array2, 0));
  ut_assert_true(ut_boolean_list_get_element(array2, 1));
  ut_assert_true(ut_boolean_list_get_element(array2, 2));

  UtObjectRef array3 = ut_boolean_array_new();
  ut_boolean_list_prepend(array3, true);
  ut_boolean_list_prepend(array3, true);
  ut_boolean_list_prepend(array3, false);
  ut_assert_int_equal(ut_list_get_length(array3), 3);
  ut_assert_false(ut_boolean_list_get_element(array3, 0));
  ut_assert_true(ut_boolean_list_get_element(array3, 1));
  ut_assert_true(ut_boolean_list_get_element(array3, 2));

  UtObjectRef array4 = ut_boolean_array_new();
  ut_boolean_list_append(array4, false);
  ut_boolean_list_append(array4, false);
  ut_boolean_list_insert(array4, 1, true);
  ut_assert_int_equal(ut_list_get_length(array4), 3);
  ut_assert_false(ut_boolean_list_get_element(array4, 0));
  ut_assert_true(ut_boolean_list_get_element(array4, 1));
  ut_assert_false(ut_boolean_list_get_element(array4, 2));

  return 0;
}
