#include "ut.h"

static UtObject *get_utf8_data(const char *value) {
  UtObjectRef string = ut_string_new(value);
  return ut_string_get_utf8(string);
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtObject *result = user_data;
  ut_list_append_list(result, data);
  return ut_list_get_length(data);
}

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
  UtObjectRef single_char_data = get_utf8_data("!");
  UtObjectRef single_char_data_stream =
      ut_list_input_stream_new(single_char_data);
  UtObjectRef single_char_encoder =
      ut_deflate_encoder_new(single_char_data_stream);
  UtObjectRef single_char_result =
      ut_input_stream_read_sync(single_char_encoder);
  ut_assert_is_not_error(single_char_result);
  ut_assert_uint8_list_equal_hex(single_char_result, "530400");

  // Encodes as "hello" (no repetition)
  UtObjectRef hello_data = get_utf8_data("hello");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_deflate_encoder_new(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "cb48cdc9c90700");

  // Encodes as "hello " rep(6,11)
  UtObjectRef hello3_data = get_utf8_data("hello hello hello");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_deflate_encoder_new(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result, "cb48cdc9c957402201");

  // Encodes as "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ="
  // rep(53,53)
  UtObjectRef alphabet_data =
      get_utf8_data("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ="
                    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ=");
  UtObjectRef alphabet_data_stream = ut_list_input_stream_new(alphabet_data);
  UtObjectRef alphabet_encoder = ut_deflate_encoder_new(alphabet_data_stream);
  UtObjectRef alphabet_result = ut_input_stream_read_sync(alphabet_encoder);
  ut_assert_is_not_error(alphabet_result);
  ut_assert_uint8_list_equal_hex(
      alphabet_result,
      "4b4c4a4e494d4bcfc8cccacec9cdcb2f282c2a2e292d2bafa8ac72747276717573f7f0f4"
      "f2f6f1f5f30f080c0a0e090d0b8f888cb2254b1300");

  // Encodes as "our f" rep(5,4) "h" rep(10,7) "tain"
  UtObjectRef repeat_phrase_data = get_utf8_data("our four hour fountain");
  UtObjectRef repeat_phrase_data_stream =
      ut_list_input_stream_new(repeat_phrase_data);
  UtObjectRef repeat_phrase_encoder =
      ut_deflate_encoder_new(repeat_phrase_data_stream);
  UtObjectRef repeat_phrase_result =
      ut_input_stream_read_sync(repeat_phrase_encoder);
  ut_assert_is_not_error(repeat_phrase_result);
  ut_assert_uint8_list_equal_hex(repeat_phrase_result,
                                 "cb2f2d524803111950565e4962661e00");

  // Encode one byte at a time.
  UtObjectRef short_write_data_stream = ut_buffered_input_stream_new();
  UtObjectRef short_write_encoder =
      ut_deflate_encoder_new(short_write_data_stream);
  UtObjectRef short_write_result = ut_uint8_array_new();
  ut_input_stream_read(short_write_encoder, read_cb, short_write_result, NULL);
  UtObjectRef short_write_data = get_utf8_data("hello");
  size_t short_write_data_length = ut_list_get_length(short_write_data);
  for (size_t i = 0; i < short_write_data_length; i++) {
    UtObjectRef data = ut_list_get_sublist(short_write_data, i, 1);
    ut_buffered_input_stream_write(short_write_data_stream, data,
                                   i == short_write_data_length - 1);
  }
  ut_assert_uint8_list_equal_hex(short_write_result, "cb48cdc9c90700");

  return 0;
}
