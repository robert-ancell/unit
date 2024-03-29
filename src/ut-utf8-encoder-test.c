#include <stdlib.h>

#include "ut.h"

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  ut_assert_int_equal(ut_list_get_length(data), 6);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 0), 72);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 1), 105);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 2), 240);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 3), 159);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 4), 152);
  ut_assert_int_equal(ut_uint8_list_get_element(data, 5), 128);
  ut_event_loop_return(NULL);
  return ut_list_get_length(data);
}

int main(int argc, char **argv) {
  UtObjectRef code_points =
      ut_uint32_array_new_from_elements(3, 'H', 'i', 0x1f600);
  UtObjectRef code_points_stream = ut_list_input_stream_new(code_points);
  UtObjectRef utf8_encoder = ut_utf8_encoder_new(code_points_stream);
  UtObjectRef dummy_object = ut_null_new();
  ut_input_stream_read_all(utf8_encoder, dummy_object, read_cb);

  ut_event_loop_run();

  return 0;
}
