#include "ut.h"

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  ut_list_append_list(object, data);
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

  UtObjectRef alphabet_data = ut_uint8_list_new_from_hex_string(
      "4b4c4a4e494d4bcfc8cccacec9cdcb2f282c2a2e292d2bafa8ac72747276717573f7f0f4"
      "f2f6f1f5f30f080c0a0e090d0b8f888cb2254b1300");
  UtObjectRef alphabet_data_stream = ut_list_input_stream_new(alphabet_data);
  UtObjectRef alphabet_decoder = ut_deflate_decoder_new(alphabet_data_stream);
  UtObjectRef alphabet_result = ut_input_stream_read_sync(alphabet_decoder);
  ut_assert_is_not_error(alphabet_result);
  UtObjectRef alphabet_result_string = ut_string_new_from_utf8(alphabet_result);
  ut_assert_cstring_equal(
      ut_string_get_text(alphabet_result_string),
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ="
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ=");

  UtObjectRef literal_data = ut_uint8_list_new_from_hex_string("010100feff21");
  UtObjectRef literal_data_stream = ut_list_input_stream_new(literal_data);
  UtObjectRef literal_decoder = ut_deflate_decoder_new(literal_data_stream);
  UtObjectRef literal_result = ut_input_stream_read_sync(literal_decoder);
  ut_assert_is_not_error(literal_result);
  UtObjectRef literal_result_string = ut_string_new_from_utf8(literal_result);
  ut_assert_cstring_equal(ut_string_get_text(literal_result_string), "!");

  // Data encoded using a dynamic Huffman table.
  UtObjectRef dynamic_huffman_data = ut_uint8_list_new_from_hex_string(
      "1dc6490100001040c0aca37f883d3c202a979d375e1d0c");
  UtObjectRef dynamic_huffman_data_stream =
      ut_list_input_stream_new(dynamic_huffman_data);
  UtObjectRef dynamic_huffman_decoder =
      ut_deflate_decoder_new(dynamic_huffman_data_stream);
  UtObjectRef dynamic_huffman_result =
      ut_input_stream_read_sync(dynamic_huffman_decoder);
  ut_assert_is_not_error(dynamic_huffman_result);
  UtObjectRef dynamic_huffman_result_string =
      ut_string_new_from_utf8(dynamic_huffman_result);
  ut_assert_cstring_equal(ut_string_get_text(dynamic_huffman_result_string),
                          "abaabbbabaababbaababaaaabaaabbbbbaa");

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
  UtObjectRef short_write_data_stream = ut_buffered_input_stream_new();
  UtObjectRef short_write_decoder =
      ut_deflate_decoder_new(short_write_data_stream);
  UtObjectRef short_write_result = ut_uint8_array_new();
  ut_input_stream_read(short_write_decoder, short_write_result, read_cb);
  UtObjectRef short_write_data =
      ut_uint8_list_new_from_hex_string("cb48cdc9c90700");
  size_t short_write_data_length = ut_list_get_length(short_write_data);
  for (size_t i = 0; i < short_write_data_length; i++) {
    UtObjectRef data = ut_list_get_sublist(short_write_data, i, 1);
    ut_buffered_input_stream_write(short_write_data_stream, data,
                                   i == short_write_data_length - 1);
  }
  UtObjectRef short_write_result_string =
      ut_string_new_from_utf8(short_write_result);
  ut_assert_cstring_equal(ut_string_get_text(short_write_result_string),
                          "hello");

  return 0;
}
