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
  UtObjectRef window_size_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_DEFAULT, 256, window_size_data_stream);
  UtObjectRef window_size_result =
      ut_input_stream_read_sync(window_size_encoder);
  ut_assert_is_not_error(window_size_result);
  ut_assert_uint8_list_equal_hex(window_size_result,
                                 "0899bbb776df7b0007ee0339");

  UtObjectRef level0_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef level0_data_stream = ut_list_input_stream_new(level0_data);
  UtObjectRef level0_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_FASTEST, 32768, level0_data_stream);
  UtObjectRef level0_result = ut_input_stream_read_sync(level0_encoder);
  ut_assert_is_not_error(level0_result);
  ut_assert_uint8_list_equal_hex(level0_result, "78016360000000020001");

  UtObjectRef level1_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef level1_data_stream = ut_list_input_stream_new(level1_data);
  UtObjectRef level1_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_FAST, 32768, level1_data_stream);
  UtObjectRef level1_result = ut_input_stream_read_sync(level1_encoder);
  ut_assert_is_not_error(level1_result);
  ut_assert_uint8_list_equal_hex(level1_result, "785e6360000000020001");

  UtObjectRef level2_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef level2_data_stream = ut_list_input_stream_new(level2_data);
  UtObjectRef level2_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_DEFAULT, 32768, level2_data_stream);
  UtObjectRef level2_result = ut_input_stream_read_sync(level2_encoder);
  ut_assert_is_not_error(level2_result);
  ut_assert_uint8_list_equal_hex(level2_result, "789c6360000000020001");

  UtObjectRef level3_data = ut_uint8_list_new_from_hex_string("0000");
  UtObjectRef level3_data_stream = ut_list_input_stream_new(level3_data);
  UtObjectRef level3_encoder = ut_zlib_encoder_new_full(
      UT_ZLIB_COMPRESSION_LEVEL_MAXIMUM, 32768, level3_data_stream);
  UtObjectRef level3_result = ut_input_stream_read_sync(level3_encoder);
  ut_assert_is_not_error(level3_result);
  ut_assert_uint8_list_equal_hex(level3_result, "78da6360000000020001");

  return 0;
}
