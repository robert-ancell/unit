#include "ut.h"

static UtObject *get_utf8_data(const char *value) {
  UtObjectRef string = ut_string_new(value);
  return ut_string_get_utf8(string);
}

static void test_lsb() {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder = ut_lzw_encoder_new_lsb(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "800202");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder = ut_lzw_encoder_new_lsb(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "80000404");

  UtObjectRef hello_data = get_utf8_data("hello");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_lzw_encoder_new_lsb(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "80d09461c3e64d40");

  UtObjectRef hello3_data = get_utf8_data("hello hello hello");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_lzw_encoder_new_lsb(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result,
                                 "80d09461c3e60d0881040d222c1810");
}

static void test_msb() {
  UtObjectRef empty_data = ut_uint8_list_new();
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_encoder = ut_lzw_encoder_new_msb(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_encoder);
  ut_assert_is_not_error(empty_result);
  ut_assert_uint8_list_equal_hex(empty_result, "804040");

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_encoder = ut_lzw_encoder_new_msb(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_encoder);
  ut_assert_is_not_error(single_result);
  ut_assert_uint8_list_equal_hex(single_result, "80002020");

  UtObjectRef hello_data = get_utf8_data("hello");
  UtObjectRef hello_data_stream = ut_list_input_stream_new(hello_data);
  UtObjectRef hello_encoder = ut_lzw_encoder_new_msb(hello_data_stream);
  UtObjectRef hello_result = ut_input_stream_read_sync(hello_encoder);
  ut_assert_is_not_error(hello_result);
  ut_assert_uint8_list_equal_hex(hello_result, "801a0ca6c361be02");

  UtObjectRef hello3_data = get_utf8_data("hello hello hello");
  UtObjectRef hello3_data_stream = ut_list_input_stream_new(hello3_data);
  UtObjectRef hello3_encoder = ut_lzw_encoder_new_msb(hello3_data_stream);
  UtObjectRef hello3_result = ut_input_stream_read_sync(hello3_encoder);
  ut_assert_is_not_error(hello3_result);
  ut_assert_uint8_list_equal_hex(hello3_result,
                                 "801a0ca6c361bc41028241a1105808");
}

int main(int argc, char **argv) {
  test_lsb();
  test_msb();

  return 0;
}
