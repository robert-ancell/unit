#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  int32_t expected_list[3] = {0, INT32_MIN, INT32_MAX};

  UtObjectRef array0 = ut_int32_array_new();
  ut_assert_int32_list_equal(array0, NULL, 0);

  UtObjectRef array1 =
      ut_int32_array_new_from_elements(3, 0, INT32_MIN, INT32_MAX);
  ut_assert_int32_list_equal(array1, expected_list, 3);

  UtObjectRef array2 = ut_int32_array_new();
  ut_int32_list_append(array2, 0);
  ut_int32_list_append(array2, INT32_MIN);
  ut_int32_list_append(array2, INT32_MAX);
  ut_assert_int32_list_equal(array2, expected_list, 3);

  UtObjectRef array3 = ut_int32_array_new();
  ut_int32_list_prepend(array3, INT32_MAX);
  ut_int32_list_prepend(array3, INT32_MIN);
  ut_int32_list_prepend(array3, 0);
  ut_assert_int32_list_equal(array3, expected_list, 3);

  UtObjectRef array4 = ut_int32_array_new();
  ut_int32_list_append(array4, 0);
  ut_int32_list_append(array4, INT32_MAX);
  ut_int32_list_insert(array4, 1, INT32_MIN);
  ut_assert_int32_list_equal(array4, expected_list, 3);

  return 0;
}
