#include "ut.h"

static void test_append() {
  // Append individual bits.
  UtObjectRef array1 = ut_bit_array_new();
  ut_bit_list_append(array1, true);
  ut_bit_list_append(array1, true);
  ut_bit_list_append(array1, false);
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(array1);
  ut_assert_cstring_equal(string1, "110");

  // Append a bytes worth of bits.
  UtObjectRef array2 = ut_bit_array_new();
  ut_bit_list_append_bits(array2, 0x2a, 8);
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(array2);
  ut_assert_cstring_equal(string2, "00101010");

  // Append half a byte (upper bits are ignored).
  UtObjectRef array3 = ut_bit_array_new();
  ut_bit_list_append_bits(array3, 0xf2, 4);
  ut_cstring_ref string3 = ut_bit_list_to_bin_string(array3);
  ut_assert_cstring_equal(string3, "0010");

  // Mix individual bits and multiple bits.
  UtObjectRef array4 = ut_bit_array_new();
  ut_bit_list_append(array4, true);
  ut_bit_list_append_bits(array4, 0x2a, 8);
  ut_cstring_ref string4 = ut_bit_list_to_bin_string(array4);
  ut_assert_cstring_equal(string4, "100101010");
}

static void test_prepend() {
  // Prepend individual bits.
  UtObjectRef array1 = ut_bit_array_new();
  ut_bit_list_prepend(array1, true);
  ut_bit_list_prepend(array1, true);
  ut_bit_list_prepend(array1, false);
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(array1);
  ut_assert_cstring_equal(string1, "011");

  // Prepend a bytes worth of bits.
  UtObjectRef array2 = ut_bit_array_new();
  ut_bit_list_prepend_bits(array2, 0x2a, 8);
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(array2);
  ut_assert_cstring_equal(string2, "00101010");

  // Prepend half a byte (upper bits are ignored).
  UtObjectRef array3 = ut_bit_array_new();
  ut_bit_list_prepend_bits(array3, 0xf2, 4);
  ut_cstring_ref string3 = ut_bit_list_to_bin_string(array3);
  ut_assert_cstring_equal(string3, "0010");

  // Mix individual bits and multiple bits.
  UtObjectRef array4 = ut_bit_array_new();
  ut_bit_list_prepend(array4, true);
  ut_bit_list_prepend_bits(array4, 0x2a, 8);
  ut_cstring_ref string4 = ut_bit_list_to_bin_string(array4);
  ut_assert_cstring_equal(string4, "001010101");
}

static void test_insert() {
  // Insert into empty array.
  UtObjectRef array1 = ut_bit_array_new();
  ut_bit_list_insert(array1, 0, true);
  ut_bit_list_insert(array1, 1, false);
  ut_bit_list_insert(array1, 0, true);
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(array1);
  ut_assert_cstring_equal(string1, "110");

  // Insert a bytes worth of bits.
  UtObjectRef array2 = ut_bit_array_new();
  ut_bit_list_insert_bits(array2, 0, 0x2a, 8);
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(array2);
  ut_assert_cstring_equal(string2, "00101010");

  // Insert between elements.
  UtObjectRef array3 = ut_bit_array_new_from_bin_string("01100110");
  ut_bit_list_insert_bits(array3, 4, 0xf, 4);
  ut_cstring_ref string3 = ut_bit_list_to_bin_string(array3);
  ut_assert_cstring_equal(string3, "011011110110");
}

static void test_remove() {
  // Remove from the middle.
  UtObjectRef array1 = ut_bit_array_new_from_bin_string("011011110110");
  ut_list_remove(array1, 4, 4);
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(array1);
  ut_assert_cstring_equal(string1, "01100110");

  // Remove on byte boundary.
  UtObjectRef array2 =
      ut_bit_array_new_from_bin_string("010101011111111110101010");
  ut_list_remove(array2, 8, 8);
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(array2);
  ut_assert_cstring_equal(string2, "0101010110101010");
}

int main(int argc, char **argv) {
  // Empty array.
  UtObjectRef array1 = ut_bit_array_new();
  ut_cstring_ref string1 = ut_bit_list_to_bin_string(array1);
  ut_assert_cstring_equal(string1, "");

  // Data provided in binary string form.
  UtObjectRef array2 = ut_bit_array_new_from_bin_string("100101010");
  ut_cstring_ref string2 = ut_bit_list_to_bin_string(array2);
  ut_assert_cstring_equal(string2, "100101010");

  test_append();
  test_prepend();
  test_insert();
  test_remove();

  return 0;
}
