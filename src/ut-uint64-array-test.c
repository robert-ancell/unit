#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  uint64_t expected_list[3] = {0, 0x1234567812345678, UINT64_MAX};

  UtObjectRef array0 = ut_uint64_array_new();
  ut_assert_uint64_list_equal(array0, NULL, 0);

  UtObjectRef array1 =
      ut_uint64_array_new_from_elements(3, 0, 0x1234567812345678, UINT64_MAX);
  ut_assert_uint64_list_equal(array1, expected_list, 3);

  UtObjectRef array2 = ut_uint64_array_new();
  ut_uint64_list_append(array2, 0);
  ut_uint64_list_append(array2, 0x1234567812345678);
  ut_uint64_list_append(array2, UINT64_MAX);
  ut_assert_uint64_list_equal(array2, expected_list, 3);

  UtObjectRef array3 = ut_uint64_array_new();
  ut_uint64_list_prepend(array3, UINT64_MAX);
  ut_uint64_list_prepend(array3, 0x1234567812345678);
  ut_uint64_list_prepend(array3, 0);
  ut_assert_uint64_list_equal(array3, expected_list, 3);

  UtObjectRef array4 = ut_uint64_array_new();
  ut_uint64_list_append(array4, 0);
  ut_uint64_list_append(array4, UINT64_MAX);
  ut_uint64_list_insert(array4, 1, 0x1234567812345678);
  ut_assert_uint64_list_equal(array4, expected_list, 3);

  return 0;
}
