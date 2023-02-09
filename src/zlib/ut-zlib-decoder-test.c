#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef empty_data =
      ut_uint8_list_new_from_hex_string("789c030000000001");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_zlib_decoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  assert(ut_list_get_length(empty_result) == 0);

  UtObjectRef single_data =
      ut_uint8_list_new_from_hex_string("789c63000000010001");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_zlib_decoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef double_data =
      ut_uint8_list_new_from_hex_string("789c6360000000020001");
  UtObjectRef double_data_stream = ut_list_input_stream_new(double_data);
  UtObjectRef double_decoder = ut_zlib_decoder_new(double_data_stream);
  UtObjectRef double_result = ut_input_stream_read_sync(double_decoder);
  ut_assert_is_not_error(double_result);
  ut_assert_uint8_list_equal_hex(double_result, "0000");

  UtObjectRef single_char_data =
      ut_uint8_list_new_from_hex_string("789c53040000220022");
  UtObjectRef single_char_data_stream =
      ut_list_input_stream_new(single_char_data);
  UtObjectRef single_char_decoder =
      ut_zlib_decoder_new(single_char_data_stream);
  UtObjectRef single_char_result =
      ut_input_stream_read_sync(single_char_decoder);
  ut_assert_is_not_error(single_char_result);
  UtObjectRef single_char_result_string =
      ut_string_new_from_utf8(single_char_result);
  ut_assert_cstring_equal(ut_string_get_text(single_char_result_string), "!");

  UtObjectRef hello_data =
      ut_uint8_list_new_from_hex_string("789ccb48cdc9c90700062c0215");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_zlib_decoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("789ccb48cdc9c957c84090003a2e067d");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_zlib_decoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");

  UtObjectRef window_size_data =
      ut_uint8_list_new_from_hex_string("0899bbb776df7b0007ee0339");
  UtObjectRef window_size_data_stream =
      ut_list_input_stream_new(window_size_data);
  UtObjectRef window_size_decoder =
      ut_zlib_decoder_new(window_size_data_stream);
  UtObjectRef window_size_result =
      ut_input_stream_read_sync(window_size_decoder);
  ut_assert_is_not_error(window_size_result);
  ut_assert_uint8_list_equal_hex(window_size_result, "DEADBEEF");

  // Compression level 0 (fastest)
  UtObjectRef level0_data_data =
      ut_uint8_list_new_from_hex_string("78016360000000020001");
  UtObjectRef level0_data_data_stream =
      ut_list_input_stream_new(level0_data_data);
  UtObjectRef level0_data_decoder =
      ut_zlib_decoder_new(level0_data_data_stream);
  UtObjectRef level0_data_result =
      ut_input_stream_read_sync(level0_data_decoder);
  ut_assert_is_not_error(level0_data_result);
  ut_assert_int_equal(
      ut_zlib_decoder_get_compression_level(level0_data_decoder), 0);

  // Compression level 2 (default)
  UtObjectRef level2_data_data =
      ut_uint8_list_new_from_hex_string("789c6360000000020001");
  UtObjectRef level2_data_data_stream =
      ut_list_input_stream_new(level2_data_data);
  UtObjectRef level2_data_decoder =
      ut_zlib_decoder_new(level2_data_data_stream);
  UtObjectRef level2_data_result =
      ut_input_stream_read_sync(level2_data_decoder);
  ut_assert_is_not_error(level2_data_result);
  ut_assert_int_equal(
      ut_zlib_decoder_get_compression_level(level2_data_decoder), 2);

  // Compression level 3 (maximum)
  UtObjectRef level3_data_data =
      ut_uint8_list_new_from_hex_string("78da6360000000020001");
  UtObjectRef level3_data_data_stream =
      ut_list_input_stream_new(level3_data_data);
  UtObjectRef level3_data_decoder =
      ut_zlib_decoder_new(level3_data_data_stream);
  UtObjectRef level3_data_result =
      ut_input_stream_read_sync(level3_data_decoder);
  ut_assert_is_not_error(level3_data_result);
  ut_assert_int_equal(
      ut_zlib_decoder_get_compression_level(level3_data_decoder), 3);

  return 0;
}
