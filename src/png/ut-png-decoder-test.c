#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef greyscale_1x1_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d49484452000000010000000108000000003a7e9b55000000"
      "0a4944415408996360000000020001f47164a60000000049454e44ae426082");
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
  ut_assert_uint8_list_equal_hex(ut_png_image_get_data(greyscale_1x1_image),
                                 "00");

  UtObjectRef greyscale_2x2_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d494844520000000200000002080000000057dd52f8000000"
      "0e4944415408996360a86738f31f000499024bfdba2c2a0000000049454e44ae426082");
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
  ut_assert_uint8_list_equal_hex(ut_png_image_get_data(greyscale_2x2_image),
                                 "007fccff");

  UtObjectRef rgb_2x2_data = ut_uint8_list_new_from_hex_string(
      "89504e470d0a1a0a0000000d4948445200000002000000020802000000"
      "fdd49a730000001149444154089963f8cfc000c60cff8100001fee05fb"
      "f491978b0000000049454e44ae426082");
  UtObjectRef rgb_2x2_data_stream = ut_list_input_stream_new(rgb_2x2_data);
  UtObjectRef rgb_2x2_decoder = ut_png_decoder_new(rgb_2x2_data_stream);
  UtObjectRef rgb_2x2_image = ut_png_decoder_decode_sync(rgb_2x2_decoder);
  ut_assert_is_not_error(rgb_2x2_image);
  assert(ut_png_image_get_width(rgb_2x2_image) == 2);
  assert(ut_png_image_get_height(rgb_2x2_image) == 2);
  assert(ut_png_image_get_bit_depth(rgb_2x2_image) == 8);
  assert(ut_png_image_get_colour_type(rgb_2x2_image) ==
         UT_PNG_COLOUR_TYPE_TRUECOLOUR);
  ut_assert_uint8_list_equal_hex(ut_png_image_get_data(rgb_2x2_image),
                                 "ff000000ff000000ffffffff");

  return 0;
}
