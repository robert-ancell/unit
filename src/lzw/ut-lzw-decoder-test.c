#include "ut.h"

// FIXME: Add test for when codes change from 9 to 10 bits.
// FIXME: Add test for when codes go to 13 bits and reset.
// FIXME: Add test for get code for next entry not yet in dictionary.

static void test_lsb() {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("000302");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_lzw_decoder_new_lsb(256, empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00010404");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_lzw_decoder_new_lsb(256, single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef hello_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e64d40");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_lzw_decoder_new_lsb(256, hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e60d0881040d222c1810");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_lzw_decoder_new_lsb(256, hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");

  // No clear code at start.
  UtObjectRef no_clear_data =
      ut_uint8_list_new_from_hex_string("68cab061f32620");
  UtObjectRef no_clear_data_stream = ut_list_input_stream_new(no_clear_data);
  UtObjectRef no_clear_decoder =
      ut_lzw_decoder_new_lsb(256, no_clear_data_stream);
  UtObjectRef no_clear_result = ut_input_stream_read_sync(no_clear_decoder);
  ut_assert_is_not_error(no_clear_result);
  UtObjectRef no_clear_result_string = ut_string_new_from_utf8(no_clear_result);
  ut_assert_cstring_equal(ut_string_get_text(no_clear_result_string), "hello");

  // Two clear codes at start.
  UtObjectRef double_clear_data =
      ut_uint8_list_new_from_hex_string("0001a229c386cd9b80");
  UtObjectRef double_clear_data_stream =
      ut_list_input_stream_new(double_clear_data);
  UtObjectRef double_clear_decoder =
      ut_lzw_decoder_new_lsb(256, double_clear_data_stream);
  UtObjectRef double_clear_result =
      ut_input_stream_read_sync(double_clear_decoder);
  ut_assert_is_not_error(double_clear_result);
  UtObjectRef double_clear_result_string =
      ut_string_new_from_utf8(double_clear_result);
  ut_assert_cstring_equal(ut_string_get_text(double_clear_result_string),
                          "hello");

  // Clear codes inside data (before spaces).
  UtObjectRef clear_during_data = ut_uint8_list_new_from_hex_string(
      "00d19461c3e60d401068cab061f3462041830101");
  UtObjectRef clear_during_data_stream =
      ut_list_input_stream_new(clear_during_data);
  UtObjectRef clear_during_decoder =
      ut_lzw_decoder_new_lsb(256, clear_during_data_stream);
  UtObjectRef clear_during_result =
      ut_input_stream_read_sync(clear_during_decoder);
  ut_assert_is_not_error(clear_during_result);
  UtObjectRef clear_during_result_string =
      ut_string_new_from_utf8(clear_during_result);
  ut_assert_cstring_equal(ut_string_get_text(clear_during_result_string),
                          "hello hello hello");

  // Data after end of information.
  UtObjectRef data_after_eoi_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e64dc0ffff");
  UtObjectRef data_after_eoi_data_stream =
      ut_list_input_stream_new(data_after_eoi_data);
  UtObjectRef data_after_eoi_decoder =
      ut_lzw_decoder_new_lsb(256, data_after_eoi_data_stream);
  UtObjectRef data_after_eoi_result =
      ut_input_stream_read_sync(data_after_eoi_decoder);
  ut_assert_is_not_error(data_after_eoi_result);
  UtObjectRef data_after_eoi_result_string =
      ut_string_new_from_utf8(data_after_eoi_result);
  ut_assert_cstring_equal(ut_string_get_text(data_after_eoi_result_string),
                          "hello");

  // No end of information.
  UtObjectRef no_eoi_data = ut_uint8_list_new_from_hex_string("00d19461c3e60d");
  UtObjectRef no_eoi_data_stream = ut_list_input_stream_new(no_eoi_data);
  UtObjectRef no_eoi_decoder = ut_lzw_decoder_new_lsb(256, no_eoi_data_stream);
  UtObjectRef no_eoi_result = ut_input_stream_read_sync(no_eoi_decoder);
  ut_assert_is_error(no_eoi_result);

  // Invalid code (encoded "hello" with code for o set to 511).
  UtObjectRef invalid_code_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e67f40");
  UtObjectRef invalid_code_data_stream =
      ut_list_input_stream_new(invalid_code_data);
  UtObjectRef invalid_code_decoder =
      ut_lzw_decoder_new_lsb(256, invalid_code_data_stream);
  UtObjectRef invalid_code_result =
      ut_input_stream_read_sync(invalid_code_decoder);
  ut_assert_is_error(invalid_code_result);
}

static void test_msb() {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("804040");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_lzw_decoder_new_msb(256, empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("80002020");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_lzw_decoder_new_msb(256, single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef hello_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361be02");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_lzw_decoder_new_msb(256, hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361bc41028241a1105808");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_lzw_decoder_new_msb(256, hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");

  // No clear code at start.
  UtObjectRef no_clear_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361be02");
  UtObjectRef no_clear_data_stream = ut_list_input_stream_new(no_clear_data);
  UtObjectRef no_clear_decoder =
      ut_lzw_decoder_new_msb(256, no_clear_data_stream);
  UtObjectRef no_clear_result = ut_input_stream_read_sync(no_clear_decoder);
  ut_assert_is_not_error(no_clear_result);
  UtObjectRef no_clear_result_string = ut_string_new_from_utf8(no_clear_result);
  ut_assert_cstring_equal(ut_string_get_text(no_clear_result_string), "hello");

  // Two clear codes at start.
  UtObjectRef double_clear_data =
      ut_uint8_list_new_from_hex_string("80400d065361b0df01");
  UtObjectRef double_clear_data_stream =
      ut_list_input_stream_new(double_clear_data);
  UtObjectRef double_clear_decoder =
      ut_lzw_decoder_new_msb(256, double_clear_data_stream);
  UtObjectRef double_clear_result =
      ut_input_stream_read_sync(double_clear_decoder);
  ut_assert_is_not_error(double_clear_result);
  UtObjectRef double_clear_result_string =
      ut_string_new_from_utf8(double_clear_result);
  ut_assert_cstring_equal(ut_string_get_text(double_clear_result_string),
                          "hello");

  // Clear codes inside data (before spaces).
  UtObjectRef clear_during_data = ut_uint8_list_new_from_hex_string(
      "801a0ca6c361be002034194d86c37c0a09068080");
  UtObjectRef clear_during_data_stream =
      ut_list_input_stream_new(clear_during_data);
  UtObjectRef clear_during_decoder =
      ut_lzw_decoder_new_msb(256, clear_during_data_stream);
  UtObjectRef clear_during_result =
      ut_input_stream_read_sync(clear_during_decoder);
  ut_assert_is_not_error(clear_during_result);
  UtObjectRef clear_during_result_string =
      ut_string_new_from_utf8(clear_during_result);
  ut_assert_cstring_equal(ut_string_get_text(clear_during_result_string),
                          "hello hello hello");

  // Data after end of information.
  UtObjectRef data_after_eoi_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361be02ffff");
  UtObjectRef data_after_eoi_data_stream =
      ut_list_input_stream_new(data_after_eoi_data);
  UtObjectRef data_after_eoi_decoder =
      ut_lzw_decoder_new_msb(256, data_after_eoi_data_stream);
  UtObjectRef data_after_eoi_result =
      ut_input_stream_read_sync(data_after_eoi_decoder);
  ut_assert_is_not_error(data_after_eoi_result);
  UtObjectRef data_after_eoi_result_string =
      ut_string_new_from_utf8(data_after_eoi_result);
  ut_assert_cstring_equal(ut_string_get_text(data_after_eoi_result_string),
                          "hello");

  // No end of information.
  UtObjectRef no_eoi_data = ut_uint8_list_new_from_hex_string("801a0ca6c361bc");
  UtObjectRef no_eoi_data_stream = ut_list_input_stream_new(no_eoi_data);
  UtObjectRef no_eoi_decoder = ut_lzw_decoder_new_msb(256, no_eoi_data_stream);
  UtObjectRef no_eoi_result = ut_input_stream_read_sync(no_eoi_decoder);
  ut_assert_is_error(no_eoi_result);

  // Invalid code (encoded "hello" with code for o set to 511).
  UtObjectRef invalid_code_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c367fe02");
  UtObjectRef invalid_code_data_stream =
      ut_list_input_stream_new(invalid_code_data);
  UtObjectRef invalid_code_decoder =
      ut_lzw_decoder_new_lsb(256, invalid_code_data_stream);
  UtObjectRef invalid_code_result =
      ut_input_stream_read_sync(invalid_code_decoder);
  ut_assert_is_error(invalid_code_result);
}

int main(int argc, char **argv) {
  test_lsb();
  test_msb();

  return 0;
}
