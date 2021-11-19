#include "ut-jpeg-decoder-test-data.h"
#include "ut.h"

static void check_jpeg(const char *hex_data, size_t width, size_t height,
                       size_t n_components, const char *hex_image_data) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_jpeg_decoder_new(data_stream);
  UtObjectRef image = ut_jpeg_decoder_decode_sync(decoder);
  ut_assert_is_not_error(image);
  ut_assert_int_equal(ut_jpeg_image_get_width(image), width);
  ut_assert_int_equal(ut_jpeg_image_get_height(image), height);
  ut_assert_int_equal(ut_jpeg_image_get_n_components(image), n_components);
  ut_assert_uint8_list_equal_hex(ut_jpeg_image_get_data(image), hex_image_data);
}

int main(int argc, char **argv) {
  check_jpeg(ange_albertini_data, 104, 56, 1, ange_albertini_image_data);

  return 0;
}
