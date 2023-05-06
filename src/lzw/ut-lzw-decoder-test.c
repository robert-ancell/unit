#include "ut.h"

static void test_lsb() {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("000302");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_lzw_decoder_new_lsb(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00010404");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_lzw_decoder_new_lsb(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef hello_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e64d40");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_lzw_decoder_new_lsb(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("00d19461c3e60d0881040d222c1810");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_lzw_decoder_new_lsb(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");
}

static void test_msb() {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("804040");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_lzw_decoder_new_msb(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("80002020");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_lzw_decoder_new_msb(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "00");

  UtObjectRef hello_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361be02");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_decoder = ut_lzw_decoder_new_msb(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_decoder);
  ut_assert_is_not_error(hello_result);
  UtObjectRef hello_result_string = ut_string_new_from_utf8(hello_result);
  ut_assert_cstring_equal(ut_string_get_text(hello_result_string), "hello");

  UtObjectRef hello3_data =
      ut_uint8_list_new_from_hex_string("801a0ca6c361bc41028241a1105808");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_decoder = ut_lzw_decoder_new_msb(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_decoder);
  ut_assert_is_not_error(hello3_result);
  UtObjectRef hello3_result_string = ut_string_new_from_utf8(hello3_result);
  ut_assert_cstring_equal(ut_string_get_text(hello3_result_string),
                          "hello hello hello");
}

int main(int argc, char **argv) {
  test_lsb();
  test_msb();

  return 0;
}
