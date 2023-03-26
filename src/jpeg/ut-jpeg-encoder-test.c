#include <assert.h>
#include <stdio.h>

#include "ut-jpeg-encoder-test-data.h"
#include "ut.h"

static void check_jpeg(size_t width, size_t height, size_t n_components,
                       const char *hex_image_data, const char *hex_data) {
  UtObjectRef data = ut_uint8_array_new();
  UtObjectRef image_data = ut_uint8_list_new_from_hex_string(hex_image_data);
  UtObjectRef image =
      ut_jpeg_image_new(width, height, UT_JPEG_DENSITY_UNITS_NONE, 1, 1,
                        n_components, image_data);
  UtObjectRef encoder = ut_jpeg_encoder_new(image, data);
  ut_jpeg_encoder_encode(encoder);
  ut_assert_uint8_list_equal_hex(data, hex_data);
}

int main(int argc, char **argv) {
  check_jpeg(8, 8, 1, wikipedia_image_data, wikipedia_data);

  check_jpeg(32, 32, 1, test_greyscale_image_data, test_greyscale_data);

  return 0;
}
