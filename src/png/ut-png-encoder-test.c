#include <assert.h>
#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef greyscale_1x1_data = ut_uint8_array_new();
  UtObjectRef greyscale_1x1_image_data =
      ut_uint8_list_new_from_hex_string("00");
  UtObjectRef greyscale_1x1_image = ut_png_image_new(
      1, 1, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, greyscale_1x1_image_data);
  UtObjectRef greyscale_1x1_encoder =
      ut_png_encoder_new(greyscale_1x1_image, greyscale_1x1_data);
  ut_png_encoder_encode(greyscale_1x1_encoder);
  ut_assert_uint8_list_equal_hex(
      greyscale_1x1_data,
      "89504e470d0a1a0a0000000d49484452000000010000000108000000003a7e9b55000000"
      "0a4944415408996360000000020001f47164a60000000049454e44ae426082");

  UtObjectRef greyscale_2x2_data = ut_uint8_array_new();
  UtObjectRef greyscale_2x2_image_data =
      ut_uint8_list_new_from_hex_string("007fccff");
  UtObjectRef greyscale_2x2_image = ut_png_image_new(
      2, 2, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, greyscale_2x2_image_data);
  UtObjectRef greyscale_2x2_encoder =
      ut_png_encoder_new(greyscale_2x2_image, greyscale_2x2_data);
  ut_png_encoder_encode(greyscale_2x2_encoder);
  ut_assert_uint8_list_equal_hex(
      greyscale_2x2_data,
      "89504e470d0a1a0a0000000d494844520000000200000002080000000057dd52f8000000"
      "0e4944415408996360a86738f31f000499024bfdba2c2a0000000049454e44ae426082");

  UtObjectRef rgb_2x2_data = ut_uint8_array_new();
  UtObjectRef rgb_2x2_image_data =
      ut_uint8_list_new_from_hex_string("ff000000ff000000ffffffff");
  UtObjectRef rgb_2x2_image = ut_png_image_new(
      2, 2, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, rgb_2x2_image_data);
  UtObjectRef rgb_2x2_encoder = ut_png_encoder_new(rgb_2x2_image, rgb_2x2_data);
  ut_png_encoder_encode(rgb_2x2_encoder);
  ut_assert_uint8_list_equal_hex(
      rgb_2x2_data, "89504e470d0a1a0a0000000d4948445200000002000000020802000000"
                    "fdd49a730000001149444154089963f8cfc000c60cff8100001fee05fb"
                    "f491978b0000000049454e44ae426082");

  return 0;
}
