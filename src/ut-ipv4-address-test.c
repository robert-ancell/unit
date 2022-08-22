#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef a1 = ut_ipv4_address_new(0xc0a8012a);
  assert(ut_ipv4_address_get_address(a1) == 0xc0a8012a);

  UtObjectRef a2 = ut_ipv4_address_new_from_quad(192, 168, 1, 42);
  assert(ut_ipv4_address_get_address(a2) == 0xc0a8012a);

  UtObjectRef a3 = ut_ipv4_address_new_from_string("192.168.1.42");
  assert(ut_ipv4_address_get_address(a3) == 0xc0a8012a);
  UtObjectRef a4 = ut_ipv4_address_new_from_string("");
  ut_assert_is_error(a4);
  UtObjectRef a5 = ut_ipv4_address_new_from_string("192.168.1.");
  ut_assert_is_error(a5);
  UtObjectRef a6 = ut_ipv4_address_new_from_string(".168.1.55");
  ut_assert_is_error(a6);
  UtObjectRef a7 = ut_ipv4_address_new_from_string("...");
  ut_assert_is_error(a7);
  UtObjectRef a8 = ut_ipv4_address_new_from_string("192.168.1.X");
  ut_assert_is_error(a8);
  UtObjectRef a9 = ut_ipv4_address_new_from_string("192.168.1.999");
  ut_assert_is_error(a9);

  return 0;
}
