#include "ut-tiff-image-test-data.h"
#include "ut.h"

static void
check_tiff(const char *hex_data, uint32_t width, uint32_t length,
           UtTiffPhotometricInterpretation photometric_interpretation,
           uint16_t bits_per_sample, uint16_t samples_per_pixel,
           const char *hex_color_map_data, const char *hex_image_data) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef image = ut_tiff_image_new_from_data(data);
  ut_assert_is_not_error(image);
  ut_assert_int_equal(ut_tiff_image_get_width(image), width);
  ut_assert_int_equal(ut_tiff_image_get_length(image), length);
  ut_assert_int_equal(ut_tiff_image_get_photometric_interpretation(image),
                      photometric_interpretation);
  ut_assert_int_equal(ut_tiff_image_get_bits_per_sample(image),
                      bits_per_sample);
  ut_assert_int_equal(ut_tiff_image_get_samples_per_pixel(image),
                      samples_per_pixel);
  if (hex_color_map_data == NULL) {
    ut_assert_null_object(ut_tiff_image_get_color_map(image));
  } else {
    ut_assert_non_null(ut_tiff_image_get_color_map(image));
    ut_assert_uint16_list_equal_hex(ut_tiff_image_get_color_map(image),
                                    hex_color_map_data);
  }
  ut_assert_uint8_list_equal_hex(ut_tiff_image_get_data(image), hex_image_data);
}

int main(int argc, char **argv) {
  check_tiff(bilevel_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 1, 1, NULL,
             bilevel_image_data);
  check_tiff(bilevel_invert_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO, 1, 1, NULL,
             bilevel_invert_image_data);
  check_tiff(bilevel_packbits_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 1, 1, NULL,
             bilevel_packbits_image_data);

  check_tiff(grayscale4_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 4, 1, NULL,
             grayscale4_image_data);
  check_tiff(grayscale4_invert_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO, 4, 1, NULL,
             grayscale4_invert_image_data);
  // FIXME: lzw
  // FIXME: deflate

  check_tiff(grayscale8_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 8, 1, NULL,
             grayscale8_image_data);
  check_tiff(grayscale8_invert_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO, 8, 1, NULL,
             grayscale8_invert_image_data);
  check_tiff(grayscale8_alpha_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 8, 2, NULL,
             grayscale8_alpha_image_data);
  check_tiff(grayscale8_lzw_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 8, 1, NULL,
             grayscale8_lzw_image_data);
  check_tiff(grayscale8_deflate_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO, 8, 1, NULL,
             grayscale8_deflate_image_data);

  check_tiff(rgb_data, 32, 32, UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB, 8, 3,
             NULL, rgb_image_data);
  check_tiff(rgb_alpha_data, 32, 32, UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB, 8,
             4, NULL, rgb_alpha_image_data);
  check_tiff(rgb_lzw_data, 32, 32, UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB, 8, 3,
             NULL, rgb_lzw_image_data);
  check_tiff(rgb_deflate_data, 32, 32, UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB,
             8, 3, NULL, rgb_deflate_image_data);
  // FIXME: deflate without predictor, rgb planar

  check_tiff(palette4_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE, 4, 1,
             palette4_color_map_data, palette4_image_data);
  check_tiff(palette8_data, 32, 32,
             UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE, 8, 1,
             palette8_color_map_data, palette8_image_data);

  return 0;
}
