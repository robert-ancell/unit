#include <assert.h>
#include <stdio.h>

#include "ut.h"

// Check the PNG image can be encoded and matches the expected data.
static void check_png(uint32_t width, uint32_t height, uint8_t bit_depth,
                      UtPngColourType colour_type, const char *hex_palette_data,
                      const char *hex_background, const char *hex_image_data,
                      const char *hex_data) {
  UtObjectRef data = ut_uint8_array_new();
  UtObjectRef image_data = ut_uint8_list_new_from_hex_string(hex_image_data);
  UtObjectRef image =
      ut_png_image_new(width, height, bit_depth, colour_type, image_data);
  UtObjectRef encoder = ut_png_encoder_new(image, data);
  ut_png_encoder_encode(encoder);
  ut_assert_uint8_list_equal_hex(data, hex_data);
}

int main(int argc, char **argv) {
  check_png(
      1, 1, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL, "00",
      "89504e470d0a1a0a0000000d49484452000000010000000108000000003a7e9b55000000"
      "0a4944415408996360000000020001f47164a60000000049454e44ae426082");

  check_png(
      2, 2, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL, "007fccff",
      "89504e470d0a1a0a0000000d494844520000000200000002080000000057dd52f8000000"
      "0e4944415408996360a86738f31f000499024bfdba2c2a0000000049454e44ae426082");

  check_png(2, 2, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            "ff000000ff000000ffffffff",
            "89504e470d0a1a0a0000000d4948445200000002000000020802000000"
            "fdd49a730000001149444154089963f8cfc000c60cff8100001fee05fb"
            "f491978b0000000049454e44ae426082");

  return 0;
}
