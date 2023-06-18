#include "ut.h"

static UtObject *get_utf8_data(const char *value) {
  UtObjectRef string = ut_string_new(value);
  return ut_string_get_utf8(string);
}

static void test_lsb() {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder =
      ut_lzw_encoder_new_lsb(256, 4096, empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "000302");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder =
      ut_lzw_encoder_new_lsb(256, 4096, single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00010404");

  UtObjectRef hello_data = get_utf8_data("hello");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder =
      ut_lzw_encoder_new_lsb(256, 4096, hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "00d19461c3e64d40");

  UtObjectRef hello3_data = get_utf8_data("hello hello hello");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder =
      ut_lzw_encoder_new_lsb(256, 4096, hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result,
                                 "00d19461c3e60d0881040d222c1810");

  // Encode message that has code words from 3 bits to 5 bits.
  UtObjectRef code_length_data =
      ut_uint8_list_new_from_hex_string("0102030404030201040103");
  UtObjectRef code_length_data_stream =
      ut_list_input_stream_new(code_length_data);
  UtObjectRef code_length_encoder =
      ut_lzw_encoder_new_lsb(5, 4096, code_length_data_stream);
  UtObjectRef code_length_result =
      ut_input_stream_read_sync(code_length_encoder);
  ut_assert_is_not_error(code_length_result);
  ut_assert_uint8_list_equal_hex(code_length_result, "8d866824288601");

  // Encode message that uses the full dictionary and resets it.
  UtObjectRef dictionary_reset_data =
      ut_uint8_list_new_from_hex_string("0102030404030201040103");
  UtObjectRef dictionary_reset_data_stream =
      ut_list_input_stream_new(dictionary_reset_data);
  UtObjectRef dictionary_reset_encoder =
      ut_lzw_encoder_new_lsb(5, 16, dictionary_reset_data_stream);
  UtObjectRef dictionary_reset_result =
      ut_input_stream_read_sync(dictionary_reset_encoder);
  ut_assert_is_not_error(dictionary_reset_result);
  ut_assert_uint8_list_equal_hex(dictionary_reset_result, "8d866824a83203");
}

static void test_msb() {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder =
      ut_lzw_encoder_new_msb(256, 4096, empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "804040");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder =
      ut_lzw_encoder_new_msb(256, 4096, single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "80002020");

  UtObjectRef hello_data = get_utf8_data("hello");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder =
      ut_lzw_encoder_new_msb(256, 4096, hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "801a0ca6c361be02");

  UtObjectRef hello3_data = get_utf8_data("hello hello hello");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder =
      ut_lzw_encoder_new_msb(256, 4096, hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result,
                                 "801a0ca6c361bc41028241a1105808");

  // Encode message that has code words from 3 bits to 5 bits.
  UtObjectRef code_length_data =
      ut_uint8_list_new_from_hex_string("0102030404030201040103");
  UtObjectRef code_length_data_stream =
      ut_list_input_stream_new(code_length_data);
  UtObjectRef code_length_encoder =
      ut_lzw_encoder_new_msb(5, 4096, code_length_data_stream);
  UtObjectRef code_length_result =
      ut_input_stream_read_sync(code_length_encoder);
  ut_assert_is_not_error(code_length_result);
  ut_assert_uint8_list_equal_hex(code_length_result, "a51a2190a08cc0");

  // Encode message that uses the full dictionary and resets it.
  UtObjectRef dictionary_reset_data =
      ut_uint8_list_new_from_hex_string("0102030404030201040103");
  UtObjectRef dictionary_reset_data_stream =
      ut_list_input_stream_new(dictionary_reset_data);
  UtObjectRef dictionary_reset_encoder =
      ut_lzw_encoder_new_msb(5, 16, dictionary_reset_data_stream);
  UtObjectRef dictionary_reset_result =
      ut_input_stream_read_sync(dictionary_reset_encoder);
  ut_assert_is_not_error(dictionary_reset_result);
  ut_assert_uint8_list_equal_hex(dictionary_reset_result, "a51a2190a296c0");
}

int main(int argc, char **argv) {
  test_lsb();
  test_msb();

  return 0;
}
