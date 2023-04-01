#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef array0 = ut_uint8_array_new();
  ut_assert_uint8_list_equal_hex(array0, "");

  UtObjectRef array1 = ut_uint8_array_new_from_elements(3, 0x00, 0x78, 0xff);
  ut_assert_uint8_list_equal_hex(array1, "0078ff");

  UtObjectRef array2 = ut_uint8_array_new_from_hex_string("0078ff");
  ut_assert_uint8_list_equal_hex(array2, "0078ff");
  ut_cstring_ref hex_string = ut_uint8_list_to_hex_string(array2);
  ut_assert_cstring_equal(hex_string, "0078ff");

  UtObjectRef array3 = ut_uint8_array_new_from_hex_string("0078FF");
  ut_assert_uint8_list_equal_hex(array3, "0078ff");

  UtObjectRef array4 = ut_uint8_array_new_from_hex_string("0078F");
  ut_assert_is_error(array4);

  UtObjectRef array5 = ut_uint8_array_new_from_hex_string("0078FG");
  ut_assert_is_error(array5);

  UtObjectRef array6 = ut_uint8_array_new();
  ut_uint8_list_append(array6, 0x00);
  ut_uint8_list_append(array6, 0x78);
  ut_uint8_list_append(array6, 0xff);
  ut_assert_uint8_list_equal_hex(array6, "0078ff");

  UtObjectRef array7 = ut_uint8_array_new();
  ut_uint8_list_prepend(array7, 0xff);
  ut_uint8_list_prepend(array7, 0x78);
  ut_uint8_list_prepend(array7, 0x00);
  ut_assert_uint8_list_equal_hex(array7, "0078ff");

  UtObjectRef array8 = ut_uint8_array_new();
  ut_uint8_list_append(array8, 0x00);
  ut_uint8_list_append(array8, 0xff);
  ut_uint8_list_insert(array8, 1, 0x78);
  ut_assert_uint8_list_equal_hex(array8, "0078ff");

  return 0;
}
