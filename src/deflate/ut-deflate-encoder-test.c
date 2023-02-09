#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder = ut_deflate_encoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "0300");

  // Encodes as 00 (no repetition)
  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder = ut_deflate_encoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "630000");

  // Encodes as 00 rep(1,1)
  UtObjectRef double_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef double_data_stream = ut_list_input_stream_new(double_data);
  UtObjectRef double_encoder = ut_deflate_encoder_new(double_data_stream);
  UtObjectRef double_result = ut_input_stream_read_sync(double_encoder);
  ut_assert_is_not_error(double_result);
  ut_assert_uint8_list_equal_hex(double_result, "63600000");

  // Encodes as "!" (no repetition)
  UtObjectRef single_char_string = ut_string_new("!");
  UtObjectRef single_char_data = ut_string_get_utf8(single_char_string);
  UtObjectRef single_char_data_stream =
      ut_list_input_stream_new(single_char_data);
  UtObjectRef single_char_encoder =
      ut_deflate_encoder_new(single_char_data_stream);
  UtObjectRef single_char_result =
      ut_input_stream_read_sync(single_char_encoder);
  ut_assert_is_not_error(single_char_result);
  ut_assert_uint8_list_equal_hex(single_char_result, "530400");

  // Encodes as "hello" (no repetition)
  UtObjectRef hello_string = ut_string_new("hello");
  UtObjectRef hello_data = ut_string_get_utf8(hello_string);
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_deflate_encoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "cb48cdc9c90700");

  // Encodes as "hello " rep(6,11)
  UtObjectRef hello3_string = ut_string_new("hello hello hello");
  UtObjectRef hello3_data = ut_string_get_utf8(hello3_string);
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_deflate_encoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result, "cb48cdc9c957402201");

  // Encodes as "our f" rep(5,4) "h" rep(10,7) "tain"
  UtObjectRef repeat_phrase_string = ut_string_new("our four hour fountain");
  UtObjectRef repeat_phrase_data = ut_string_get_utf8(repeat_phrase_string);
  UtObjectRef repeat_phrase_data_stream =
      ut_list_input_stream_new(repeat_phrase_data);
  UtObjectRef repeat_phrase_encoder =
      ut_deflate_encoder_new(repeat_phrase_data_stream);
  UtObjectRef repeat_phrase_result =
      ut_input_stream_read_sync(repeat_phrase_encoder);
  ut_assert_is_not_error(repeat_phrase_result);
  ut_assert_uint8_list_equal_hex(repeat_phrase_result,
                                 "cb2f2d524803111950665e4962661e00");

  return 0;
}
