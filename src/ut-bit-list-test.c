#include "ut.h"

int main(int argc, char **argv) {
  // Empty list.
  UtObjectRef list1 = ut_bit_list_new_le();
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(list1);
  ut_assert_cstring_equal(string1, "");

  // Data provided in binary string form.
  UtObjectRef list2 = ut_bit_list_new_le_from_bin_string("100101010");
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(list2);
  ut_assert_cstring_equal(string2, "100101010");

  // Append individual bits.
  UtObjectRef list3 = ut_bit_list_new_le();
  ut_bit_list_append(list3, true);
  ut_bit_list_append(list3, true);
  ut_bit_list_append(list3, false);
  ut_cstring_ref string3 = ut_bit_list_to_bin_string(list3);
  ut_assert_cstring_equal(string3, "110");

  // Append a bytes worth of bits.
  UtObjectRef list4 = ut_bit_list_new_le();
  ut_bit_list_append_bits(list4, 0x2a, 8);
  ut_cstring_ref string4 = ut_bit_list_to_bin_string(list4);
  ut_assert_cstring_equal(string4, "00101010");

  // Append half a byte (upper bits are ignored).
  UtObjectRef list5 = ut_bit_list_new_le();
  ut_bit_list_append_bits(list5, 0xf2, 4);
  ut_cstring_ref string5 = ut_bit_list_to_bin_string(list5);
  ut_assert_cstring_equal(string5, "0010");

  // Mix individual bits and multiple bits.
  UtObjectRef list6 = ut_bit_list_new_le();
  ut_bit_list_append(list6, true);
  ut_bit_list_append_bits(list6, 0x2a, 8);
  ut_cstring_ref string6 = ut_bit_list_to_bin_string(list6);
  ut_assert_cstring_equal(string6, "100101010");

  return 0;
}
