#include <assert.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef empty_string = ut_string_new("");
  UtObjectRef empty_data = ut_string_get_utf8(empty_string);
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder = ut_deflate_encoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "0300");

  UtObjectRef single_string = ut_string_new("!");
  UtObjectRef single_data = ut_string_get_utf8(single_string);
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder = ut_deflate_encoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "530400");

  UtObjectRef hello_string = ut_string_new("hello");
  UtObjectRef hello_data = ut_string_get_utf8(hello_string);
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_deflate_encoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "cb48cdc9c90700");

  UtObjectRef hello3_string = ut_string_new("hello hello hello");
  UtObjectRef hello3_data = ut_string_get_utf8(hello3_string);
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_deflate_encoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result, "cb48cdc9c957402201");

  return 0;
}
