#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef empty_data = ut_uint8_list_new_from_hex_string("0300");
  UtObjectRef empty_data_stream = ut_list_input_stream_new(empty_data);
  UtObjectRef empty_decoder = ut_deflate_decoder_new(empty_data_stream);
  UtObjectRef empty_result = ut_input_stream_read_sync(empty_decoder);
  ut_assert_is_not_error(empty_result);
  assert(ut_list_get_length(empty_result) == 0);

  UtObjectRef single_data = ut_uint8_list_new_from_hex_string("530400");
  UtObjectRef single_data_stream = ut_list_input_stream_new(single_data);
  UtObjectRef single_decoder = ut_deflate_decoder_new(single_data_stream);
  UtObjectRef single_result = ut_input_stream_read_sync(single_decoder);
  ut_assert_is_not_error(single_result);
  UtObjectRef single_result_string = ut_string_new_from_utf8(single_result);
  ut_assert_cstring_equal(ut_string_get_text(single_result_string), "!");

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

  UtObjectRef literal_data =
      ut_uint8_list_new_from_hex_string("010100feff2100220022");
  UtObjectRef literal_data_stream = ut_list_input_stream_new(literal_data);
  UtObjectRef literal_decoder = ut_deflate_decoder_new(literal_data_stream);
  UtObjectRef literal_result = ut_input_stream_read_sync(literal_decoder);
  ut_assert_is_not_error(literal_result);
  UtObjectRef literal_result_string = ut_string_new_from_utf8(literal_result);
  ut_assert_cstring_equal(ut_string_get_text(literal_result_string), "!");

  return 0;
}
