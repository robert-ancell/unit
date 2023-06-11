#include "ut-gif-test-data.h"
#include "ut.h"

static void check_gif(const char *hex_data, uint16_t left, uint16_t top,
                      uint16_t width, uint16_t height,
                      const char *hex_color_table_data,
                      const char *hex_image_data) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_gif_decoder_new(data_stream);
  UtObjectRef images = ut_gif_decoder_decode_sync(decoder);
  ut_assert_is_not_error(images);
  ut_assert_int_equal(ut_list_get_length(images), 1);
  UtObjectRef image = ut_list_get_element(images, 0);
  ut_assert_int_equal(ut_gif_image_get_left(image), left);
  ut_assert_int_equal(ut_gif_image_get_top(image), top);
  ut_assert_int_equal(ut_gif_image_get_width(image), width);
  ut_assert_int_equal(ut_gif_image_get_height(image), height);
  ut_assert_uint8_list_equal_hex(ut_gif_image_get_color_table(image),
                                 hex_color_table_data);
  ut_assert_uint8_list_equal_hex(ut_gif_image_get_data(image), hex_image_data);
}

int main(int argc, char **argv) {
  // Different color table sizes.
  check_gif(depth1_data, 0, 0, 2, 1, depth1_color_table_data,
            depth1_image_data);
  check_gif(depth2_data, 0, 0, 2, 2, depth2_color_table_data,
            depth2_image_data);
  check_gif(depth3_data, 0, 0, 4, 2, depth3_color_table_data,
            depth3_image_data);
  check_gif(depth4_data, 0, 0, 4, 4, depth4_color_table_data,
            depth4_image_data);
  check_gif(depth5_data, 0, 0, 8, 4, depth5_color_table_data,
            depth5_image_data);
  check_gif(depth6_data, 0, 0, 8, 8, depth6_color_table_data,
            depth6_image_data);
  check_gif(depth7_data, 0, 0, 16, 8, depth7_color_table_data,
            depth7_image_data);
  check_gif(depth8_data, 0, 0, 16, 16, depth8_color_table_data,
            depth8_image_data);

  // Interlaced image.
  check_gif(interlace_data, 0, 0, 32, 32, interlace_color_table_data,
            interlace_image_data);

  return 0;
}
