#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  uint8_t expected_address[16] = {0x20, 0x01, 0xd,  0xb8, 0x85, 0xa3,
                                  0x0,  0x0,  0x0,  0x0,  0x8a, 0x2e,
                                  0x03, 0x70, 0x73, 0x34};
  UtObjectRef a1 = ut_ipv6_address_new(expected_address);
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a1), 8,
                              expected_address, 8);

  ut_cstring_ref text1 = ut_ip_address_to_string(a1);
  ut_assert_cstring_equal(text1, "2001:db8:85a3::8a2e:370:7334");

  // Full address
  UtObjectRef a2 = ut_ipv6_address_new_from_string(
      "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  ut_assert_is_not_error(a2);
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a2), 8,
                              expected_address, 8);

  // Remove leading zeros
  UtObjectRef a3 =
      ut_ipv6_address_new_from_string("2001:db8:85a3:0:0:8a2e:370:7334");
  ut_assert_is_not_error(a3);
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a3), 8,
                              expected_address, 8);

  // Upper case
  UtObjectRef a4 = ut_ipv6_address_new_from_string(
      "2001:0DB8:85A3:0000:0000:8A2E:0370:7334");
  ut_assert_is_not_error(a4);
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a4), 8,
                              expected_address, 8);

  // Remove sequence of zeros
  UtObjectRef a5 =
      ut_ipv6_address_new_from_string("2001:0db8:85a3::8a2e:0370:7334");
  ut_assert_is_not_error(a5);
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a5), 8,
                              expected_address, 8);

  // All zeros
  UtObjectRef a6 = ut_ipv6_address_new_from_string("::");
  ut_assert_is_not_error(a6);
  uint8_t empty_address[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a6), 8, empty_address,
                              8);
  ut_cstring_ref text6 = ut_ip_address_to_string(a6);
  ut_assert_cstring_equal(text6, "::");

  // Loopback
  UtObjectRef a7 = ut_ipv6_address_new_from_string("::1");
  ut_assert_is_not_error(a7);
  uint8_t loopback_address[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 1};
  ut_assert_uint8_array_equal(ut_ipv6_address_get_address(a7), 8,
                              loopback_address, 8);
  ut_cstring_ref text7 = ut_ip_address_to_string(a7);
  ut_assert_cstring_equal(text7, "::1");

  // Empty string
  UtObjectRef a8 = ut_ipv6_address_new_from_string("");
  ut_assert_is_error(a8);

  // Too few groups
  UtObjectRef a9 =
      ut_ipv6_address_new_from_string("2001:0db8:85a3:0000:0000:8a2e:0370");
  ut_assert_is_error(a9);

  // Too many groups
  UtObjectRef a10 = ut_ipv6_address_new_from_string(
      "2001:0db8:85a3:0000:0000:8a2e:0370:7334:ffff");
  ut_assert_is_error(a10);

  // Empty groups
  UtObjectRef a11 = ut_ipv6_address_new_from_string(":::::::");
  ut_assert_is_error(a11);

  // Duplicate zeros
  UtObjectRef a12 = ut_ipv6_address_new_from_string("2001:db8::1::1");
  ut_assert_is_error(a12);

  return 0;
}
