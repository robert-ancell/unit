#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef array0 = ut_uint16_array_new();
  ut_assert_uint16_list_equal_hex(array0, "");

  UtObjectRef array1 =
      ut_uint16_array_new_from_elements(3, 0, 0x5678, UINT16_MAX);
  ut_assert_uint16_list_equal_hex(array1, "00005678ffff");

  UtObjectRef array2 = ut_uint16_array_new_from_hex_string("00005678ffff");
  ut_assert_uint16_list_equal_hex(array2, "00005678ffff");

  UtObjectRef array3 = ut_uint16_array_new_from_hex_string("00005678FFFF");
  ut_assert_uint16_list_equal_hex(array3, "00005678ffff");

  UtObjectRef array4 = ut_uint16_array_new_from_hex_string("00005678FF");
  ut_assert_is_error(array4);

  UtObjectRef array5 = ut_uint16_array_new_from_hex_string("00005678FFFG");
  ut_assert_is_error(array5);

  UtObjectRef array6 = ut_uint16_array_new();
  ut_uint16_list_append(array6, 0);
  ut_uint16_list_append(array6, 0x5678);
  ut_uint16_list_append(array6, UINT16_MAX);
  ut_assert_uint16_list_equal_hex(array6, "00005678ffff");

  UtObjectRef array7 = ut_uint16_array_new();
  ut_uint16_list_prepend(array7, UINT16_MAX);
  ut_uint16_list_prepend(array7, 0x5678);
  ut_uint16_list_prepend(array7, 0);
  ut_assert_uint16_list_equal_hex(array7, "00005678ffff");

  UtObjectRef array8 = ut_uint16_array_new();
  ut_uint16_list_append(array8, 0);
  ut_uint16_list_append(array8, UINT16_MAX);
  ut_uint16_list_insert(array8, 1, 0x5678);
  ut_assert_uint16_list_equal_hex(array8, "00005678ffff");

  return 0;
}
