#include <stdio.h>
#include <stdlib.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef list = ut_uint8_array_new_from_elements(3, 0x00, 0x78, 0xff);
  uint8_t expected_data[] = {0x00, 0x78, 0xff};
  ut_assert_uint8_list_equal(list, expected_data, 3);

  UtObjectRef hex_list = ut_uint8_array_new_from_hex_string("0078ff");
  ut_assert_uint8_list_equal(hex_list, expected_data, 3);
  ut_cstring_ref hex_string = ut_uint8_list_to_hex_string(hex_list);
  ut_assert_cstring_equal(hex_string, "0078ff");

  UtObjectRef hex_list2 = ut_uint8_array_new_from_hex_string("0078FF");
  ut_assert_uint8_list_equal(hex_list2, expected_data, 3);

  UtObjectRef hex_list3 = ut_uint8_array_new_from_hex_string("0078F");
  ut_assert_is_error(hex_list3);

  UtObjectRef hex_list4 = ut_uint8_array_new_from_hex_string("0078FG");
  ut_assert_is_error(hex_list4);

  return 0;
}
