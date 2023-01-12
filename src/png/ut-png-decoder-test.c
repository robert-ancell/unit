#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef greyscale_1x1_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d49484452000000010000000108000000003a7e9b55000000"
      "0a4944415408d76360000000020001e221bc330000000049454e44ae426082");
  UtObjectRef greyscale_1x1_data_stream =
      ut_list_input_stream_new(greyscale_1x1_data);
  UtObjectRef greyscale_1x1_decoder =
      ut_png_decoder_new(greyscale_1x1_data_stream);
  UtObjectRef greyscale_1x1_image =
      ut_png_decoder_decode_sync(greyscale_1x1_decoder);
  ut_assert_is_not_error(greyscale_1x1_image);
  assert(ut_png_image_get_width(greyscale_1x1_image) == 1);
  assert(ut_png_image_get_height(greyscale_1x1_image) == 1);
  assert(ut_png_image_get_bit_depth(greyscale_1x1_image) == 8);
  assert(ut_png_image_get_colour_type(greyscale_1x1_image) ==
         UT_PNG_COLOUR_TYPE_GREYSCALE);
  uint8_t expected_greyscale_1x1_data[] = {0x00};
  ut_assert_uint8_list_equal(ut_png_image_get_data(greyscale_1x1_image),
                             expected_greyscale_1x1_data, 1);

  UtObjectRef greyscale_2x2_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d494844520000000200000002080000000057dd52f8000000"
      "0e4944415408d76360a86738f31f000499024bbad1b1310000000049454e44ae426082");
  UtObjectRef greyscale_2x2_data_stream =
      ut_list_input_stream_new(greyscale_2x2_data);
  UtObjectRef greyscale_2x2_decoder =
      ut_png_decoder_new(greyscale_2x2_data_stream);
  UtObjectRef greyscale_2x2_image =
      ut_png_decoder_decode_sync(greyscale_2x2_decoder);
  ut_assert_is_not_error(greyscale_2x2_image);
  assert(ut_png_image_get_width(greyscale_2x2_image) == 2);
  assert(ut_png_image_get_height(greyscale_2x2_image) == 2);
  assert(ut_png_image_get_bit_depth(greyscale_2x2_image) == 8);
  assert(ut_png_image_get_colour_type(greyscale_2x2_image) ==
         UT_PNG_COLOUR_TYPE_GREYSCALE);
  uint8_t expected_greyscale_2x2_data[] = {0x00, 0x7f, 0xcc, 0xff};
  ut_assert_uint8_list_equal(ut_png_image_get_data(greyscale_2x2_image),
                             expected_greyscale_2x2_data, 4);

  UtObjectRef rgb_2x2_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d4948445200000002000000020802000000fdd49a73000000"
      "164944415408d763f8cfc0c0f09f819181e1ffffff0c001ef604fd48c6091f0000000049"
      "454e44ae426082");
  UtObjectRef rgb_2x2_data_stream = ut_list_input_stream_new(rgb_2x2_data);
  UtObjectRef rgb_2x2_decoder = ut_png_decoder_new(rgb_2x2_data_stream);
  UtObjectRef rgb_2x2_image = ut_png_decoder_decode_sync(rgb_2x2_decoder);
  ut_assert_is_not_error(rgb_2x2_image);
  assert(ut_png_image_get_width(rgb_2x2_image) == 2);
  assert(ut_png_image_get_height(rgb_2x2_image) == 2);
  assert(ut_png_image_get_bit_depth(rgb_2x2_image) == 8);
  assert(ut_png_image_get_colour_type(rgb_2x2_image) ==
         UT_PNG_COLOUR_TYPE_TRUECOLOUR);
  assert(ut_list_get_length(ut_png_image_get_data(rgb_2x2_image)) == 12);
  uint8_t expected_rgb_2x2_data[] = {0xff, 0x00, 0x00, 0x00, 0xff, 0x00,
                                     0x00, 0x00, 0xff, 0xff, 0xff, 0xff};
  ut_assert_uint8_list_equal(ut_png_image_get_data(rgb_2x2_image),
                             expected_rgb_2x2_data, 12);

  return 0;
}
