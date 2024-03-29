#include <stdlib.h>

#include "ut.h"

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  ut_assert_int_equal(ut_list_get_length(data), 1);
  ut_assert_int_equal(ut_uint32_list_get_element(data, 0), 0x1f600);
  ut_event_loop_return(NULL);
  return ut_list_get_length(data);
}

int main(int argc, char **argv) {
  UtObjectRef sync_input = ut_uint8_list_new_from_hex_string("f09f9880");
  UtObjectRef sync_input_stream = ut_list_input_stream_new(sync_input);
  UtObjectRef sync_decoder = ut_utf8_decoder_new(sync_input_stream);
  UtObjectRef sync_result = ut_input_stream_read_sync(sync_decoder);
  ut_assert_is_not_error(sync_result);
  ut_assert_int_equal(ut_list_get_length(sync_result), 1);
  ut_assert_int_equal(ut_uint32_list_get_element(sync_result, 0), 0x1f600);

  UtObjectRef async_input = ut_uint8_list_new_from_hex_string("f09f9880");
  UtObjectRef async_input_stream = ut_list_input_stream_new(async_input);
  UtObjectRef async_decoder = ut_utf8_decoder_new(async_input_stream);
  UtObjectRef dummy_object = ut_null_new();
  ut_input_stream_read_all(async_decoder, dummy_object, read_cb);

  ut_event_loop_run();

  return 0;
}
