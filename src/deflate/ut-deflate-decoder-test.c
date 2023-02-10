#include "ut.h"

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *result = user_data;
  ut_list_append_list(result, data);
  return ut_list_get_length(data);
}

int main(int argc, char **argv) {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("0300");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_deflate_decoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_int_equal(ut_list_get_length(empty_result), 0);

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("630000");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_deflate_decoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef double_data = ut_uint8_list_new_from_hex_string("63600000");
  UtObjectRef double_data_stream = ut_list_input_stream_new(double_data);
  UtObjectRef double_decoder = ut_deflate_decoder_new(double_data_stream);
  UtObjectRef double_result = ut_input_stream_read_sync(double_decoder);
  ut_assert_is_not_error(double_result);
  ut_assert_uint8_list_equal_hex(double_result, "0000");

  UtObjectRef single_char_data = ut_uint8_list_new_from_hex_string("530400");
  UtObjectRef single_char_data_stream =
      ut_list_input_stream_new(single_char_data);
  UtObjectRef single_char_decoder =
      ut_deflate_decoder_new(single_char_data_stream);
  UtObjectRef single_char_result =
      ut_input_stream_read_sync(single_char_decoder);
  ut_assert_is_not_error(single_char_result);
  UtObjectRef single_char_result_string =
      ut_string_new_from_utf8(single_char_result);
  ut_assert_cstring_equal(ut_string_get_text(single_char_result_string), "!");

  UtObjectRef hello_data = ut_uint8_list_new_from_hex_string("cb48cdc9c90700");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_deflate_decoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("cb48cdc9c957402201");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_deflate_decoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");

  UtObjectRef literal_data = ut_uint8_list_new_from_hex_string("010100feff21");
  UtObjectRef literal_data_stream = ut_list_input_stream_new(literal_data);
  UtObjectRef literal_decoder = ut_deflate_decoder_new(literal_data_stream);
  UtObjectRef literal_result = ut_input_stream_read_sync(literal_decoder);
  ut_assert_is_not_error(literal_result);
  UtObjectRef literal_result_string = ut_string_new_from_utf8(literal_result);
  ut_assert_cstring_equal(ut_string_get_text(literal_result_string), "!");

  UtObjectRef repeat_phrase_data =
      ut_uint8_list_new_from_hex_string("cb2f2d524803111950665e4962661e00");
  UtObjectRef repeat_phrase_data_stream =
      ut_list_input_stream_new(repeat_phrase_data);
  UtObjectRef repeat_phrase_decoder =
      ut_deflate_decoder_new(repeat_phrase_data_stream);
  UtObjectRef repeat_phrase_result =
      ut_input_stream_read_sync(repeat_phrase_decoder);
  ut_assert_is_not_error(repeat_phrase_result);
  UtObjectRef repeat_phrase_result_string =
      ut_string_new_from_utf8(repeat_phrase_result);
  ut_assert_cstring_equal(ut_string_get_text(repeat_phrase_result_string),
                          "our four hour fountain");

  // Three blocks of Huffman encoded data. "hello", " ", "world"
  UtObjectRef multi_block_data =
      ut_uint8_list_new_from_hex_string("ca48cdc9c9074801b0f2fca29c1400");
  UtObjectRef multi_block_data_stream =
      ut_list_input_stream_new(multi_block_data);
  UtObjectRef multi_block_decoder =
      ut_deflate_decoder_new(multi_block_data_stream);
  UtObjectRef multi_block_result =
      ut_input_stream_read_sync(multi_block_decoder);
  ut_assert_is_not_error(multi_block_result);
  UtObjectRef multi_block_result_string =
      ut_string_new_from_utf8(multi_block_result);
  ut_assert_cstring_equal(ut_string_get_text(multi_block_result_string),
                          "hello world");

  // Decode one byte at a time.
  UtObjectRef short_write_data_stream = ut_writable_input_stream_new();
  UtObjectRef short_write_decoder =
      ut_deflate_decoder_new(short_write_data_stream);
  UtObjectRef short_write_result = ut_uint8_array_new();
  ut_input_stream_read(short_write_decoder, read_cb, short_write_result, NULL);
  UtObjectRef short_write_data =
      ut_uint8_list_new_from_hex_string("cb48cdc9c90700");
  size_t short_write_data_length = ut_list_get_length(short_write_data);
  for (size_t i = 0; i < short_write_data_length; i++) {
    UtObjectRef data = ut_list_get_sublist(short_write_data, i, 1);
    ut_assert_int_equal(
        ut_writable_input_stream_write(short_write_data_stream, data,
                                       i == short_write_data_length - 1),
        1);
  }
  UtObjectRef short_write_result_string =
      ut_string_new_from_utf8(short_write_result);
  ut_assert_cstring_equal(ut_string_get_text(short_write_result_string),
                          "hello");

  return 0;
}
