#include <assert.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef values1 = ut_uint32_list_new_from_elements(4, 1, 0, 8571, 1);
  UtObjectRef value1 = ut_object_identifier_new(values1);
  uint32_t expected_values1[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(ut_object_identifier_get_values(value1),
                              expected_values1, 4);
  ut_cstring_ref string1 = ut_object_identifier_to_string(value1);
  ut_assert_cstring_equal(string1, "1.0.8571.1");

  UtObjectRef value2 = ut_object_identifier_new_from_string("1.0.8571.1");
  uint32_t expected_values2[4] = {1, 0, 8571, 1};
  ut_assert_uint32_list_equal(ut_object_identifier_get_values(value2),
                              expected_values2, 4);

  UtObjectRef value3a = ut_object_identifier_new_from_string("1.0.8571.1");
  UtObjectRef value3b = ut_object_identifier_new_from_string("1.0.8571.1");
  ut_assert_true(ut_object_equal(value3a, value3b));

  UtObjectRef value4a = ut_object_identifier_new_from_string("1.0.8571.1");
  UtObjectRef value4b = ut_object_identifier_new_from_string("1.0.8571.2");
  ut_assert_false(ut_object_equal(value4a, value4b));

  return 0;
}
