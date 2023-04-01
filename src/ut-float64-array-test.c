#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  double expected_list[3] = {0.0, -0.125, 3.14159};

  UtObjectRef array0 = ut_float64_array_new();
  ut_assert_float64_list_equal(array0, NULL, 0);

  UtObjectRef array1 =
      ut_float64_array_new_from_elements(3, 0.0, -0.125, 3.14159);
  ut_assert_float64_list_equal(array1, expected_list, 3);

  UtObjectRef array2 = ut_float64_array_new();
  ut_float64_list_append(array2, 0.0);
  ut_float64_list_append(array2, -0.125);
  ut_float64_list_append(array2, 3.14159);
  ut_assert_float64_list_equal(array2, expected_list, 3);

  UtObjectRef array3 = ut_float64_array_new();
  ut_float64_list_prepend(array3, 3.14159);
  ut_float64_list_prepend(array3, -0.125);
  ut_float64_list_prepend(array3, 0.0);
  ut_assert_float64_list_equal(array3, expected_list, 3);

  UtObjectRef array4 = ut_float64_array_new();
  ut_float64_list_append(array4, 0.0);
  ut_float64_list_append(array4, 3.14159);
  ut_float64_list_insert(array4, 1, -0.125);
  ut_assert_float64_list_equal(array4, expected_list, 3);

  return 0;
}
