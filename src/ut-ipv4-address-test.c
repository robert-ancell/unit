#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef a1 = ut_ipv4_address_new(0xc0a8012a);
  ut_assert_int_equal(ut_ipv4_address_get_address(a1), 0xc0a8012a);

  UtObjectRef a2 = ut_ipv4_address_new_from_quad(192, 168, 1, 42);
  ut_assert_int_equal(ut_ipv4_address_get_address(a2), 0xc0a8012a);

  ut_cstring_ref text = ut_ip_address_to_string(a2);
  ut_assert_cstring_equal(text, "192.168.1.42");

  UtObjectRef a3 = ut_ipv4_address_new_from_string("192.168.1.42");
  ut_assert_is_not_error(a3);
  ut_assert_int_equal(ut_ipv4_address_get_address(a3), 0xc0a8012a);

  UtObjectRef a4 = ut_ipv4_address_new_from_string("");
  ut_assert_is_error_with_description(a4, "Empty value in IPv4 address");

  UtObjectRef a5 = ut_ipv4_address_new_from_string("192.168.1.");
  ut_assert_is_error_with_description(a5, "Empty value in IPv4 address");

  UtObjectRef a6 = ut_ipv4_address_new_from_string(".168.1.55");
  ut_assert_is_error_with_description(a6, "Empty value in IPv4 address");

  UtObjectRef a7 = ut_ipv4_address_new_from_string("...");
  ut_assert_is_error_with_description(a7, "Empty value in IPv4 address");

  UtObjectRef a8 = ut_ipv4_address_new_from_string("192.168.1.X");
  ut_assert_is_error_with_description(a8, "Invalid character in IPv4 address");

  UtObjectRef a9 = ut_ipv4_address_new_from_string("192.168.1.999");
  ut_assert_is_error_with_description(a9, "Value too large in IPv4 address");

  return 0;
}
