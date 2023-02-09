#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder = ut_zlib_encoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "789c030000000001");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder = ut_zlib_encoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "789c63000000010001");

  UtObjectRef double_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef double_data_stream = ut_list_input_stream_new(double_data);
  UtObjectRef double_encoder = ut_zlib_encoder_new(double_data_stream);
  UtObjectRef double_result = ut_input_stream_read_sync(double_encoder);
  ut_assert_is_not_error(double_result);
  ut_assert_uint8_list_equal_hex(double_result, "789c6360000000020001");

  UtObjectRef single_char_string = ut_string_new("!");
  UtObjectRef single_char_data = ut_string_get_utf8(single_char_string);
  UtObjectRef single_char_data_stream =
      ut_list_input_stream_new(single_char_data);
  UtObjectRef single_char_encoder =
      ut_zlib_encoder_new(single_char_data_stream);
  UtObjectRef single_char_result =
      ut_input_stream_read_sync(single_char_encoder);
  ut_assert_is_not_error(single_char_result);
  ut_assert_uint8_list_equal_hex(single_char_result, "789c53040000220022");

  UtObjectRef hello_string = ut_string_new("hello");
  UtObjectRef hello_data = ut_string_get_utf8(hello_string);
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_zlib_encoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "789ccb48cdc9c90700062c0215");

  UtObjectRef hello3_string = ut_string_new("hello hello hello");
  UtObjectRef hello3_data = ut_string_get_utf8(hello3_string);
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_zlib_encoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result,
                                 "789ccb48cdc9c9574022013a2e067d");

  UtObjectRef window_size_data = ut_uint8_list_new_from_hex_string("DEADBEEF");
  UtObjectRef window_size_data_stream =
      ut_list_input_stream_new(window_size_data);
  UtObjectRef window_size_encoder =
      ut_zlib_encoder_new_with_window_size(256, window_size_data_stream);
  UtObjectRef window_size_result =
      ut_input_stream_read_sync(window_size_encoder);
  ut_assert_is_not_error(window_size_result);
  ut_assert_uint8_list_equal_hex(window_size_result,
                                 "0899bbb776df7b0007ee0339");

  return 0;
}
