#include "ut-png-decoder-test-data.h"
#include "ut.h"

// Check the PNG image in [hex_data] can be decoded and matches the expected
// properties.
static UtObject *check_png_full(const char *hex_data, uint32_t width,
                                uint32_t height, uint8_t bit_depth,
                                UtPngColourType colour_type,
                                const char *hex_palette_data,
                                const char *hex_background,
                                const char *hex_image_data) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_png_decoder_new(data_stream);
  UtObjectRef image = ut_png_decoder_decode_sync(decoder);
  ut_assert_is_not_error(image);
  ut_assert_int_equal(ut_png_image_get_width(image), width);
  ut_assert_int_equal(ut_png_image_get_height(image), height);
  ut_assert_int_equal(ut_png_image_get_bit_depth(image), bit_depth);
  ut_assert_int_equal(ut_png_image_get_colour_type(image), colour_type);
  if (hex_background == NULL) {
    ut_assert_null(ut_png_image_get_background_colour(image));
  } else {
    ut_assert_non_null(ut_png_image_get_background_colour(image));
    ut_assert_uint8_list_equal_hex(ut_png_image_get_background_colour(image),
                                   hex_background);
  }
  if (hex_palette_data == NULL) {
    ut_assert_null(ut_png_image_get_palette(image));
  } else {
    ut_assert_non_null(ut_png_image_get_palette(image));
    ut_assert_uint8_list_equal_hex(ut_png_image_get_palette(image),
                                   hex_palette_data);
  }
  ut_assert_uint8_list_equal_hex(ut_png_image_get_data(image), hex_image_data);

  return ut_object_ref(image);
}

// Check the PNG image in [hex_data] can be decoded and matches the expected
// properties.
static void check_png(const char *hex_data, uint32_t width, uint32_t height,
                      uint8_t bit_depth, UtPngColourType colour_type,
                      const char *hex_palette_data, const char *hex_background,
                      const char *hex_image_data) {
  UtObjectRef image =
      check_png_full(hex_data, width, height, bit_depth, colour_type,
                     hex_palette_data, hex_background, hex_image_data);
}

// Check the PNG image in [hex_data] is invalid.
static void check_corrupt_png(const char *hex_data) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_png_decoder_new(data_stream);
  UtObjectRef image = ut_png_decoder_decode_sync(decoder);
  ut_assert_is_error(image);
}

static void test_png_suite_basic_formats() {
  // black & white
  check_png(basn0g01_data, 32, 32, 1, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basn0g01_image_data);

  // 2 bit (4 level) grayscale
  check_png(basn0g02_data, 32, 32, 2, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basn0g02_image_data);

  // 4 bit (16 level) grayscale
  check_png(basn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basn0g04_image_data);

  // 8 bit (256 level) grayscale
  check_png(basn0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basn0g08_image_data);

  // 16 bit (64k level) grayscale
  check_png(basn0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basn0g16_image_data);

  // 3x8 bits rgb color
  check_png(basn2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            basn2c08_image_data);

  // 3x16 bits rgb color
  check_png(basn2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, basn2c16_image_data);

  // 1 bit (2 color) paletted
  check_png(basn3p01_data, 32, 32, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basn3p01_palette_data, NULL, basn3p01_image_data);

  // 2 bit (4 color) paletted
  check_png(basn3p02_data, 32, 32, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basn3p02_palette_data, NULL, basn3p02_image_data);

  // 4 bit (16 color) paletted
  check_png(basn3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basn3p04_palette_data, NULL, basn3p04_image_data);

  // 8 bit (256 color) paletted
  check_png(basn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basn3p08_palette_data, NULL, basn3p08_image_data);

  // 8 bit grayscale + 8 bit alpha-channel
  check_png(basn4a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, NULL, basn4a08_image_data);

  // 16 bit grayscale + 16 bit alpha-channel
  check_png(basn4a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, NULL, basn4a16_image_data);

  // 3x8 bits rgb color + 8 bit alpha-channel
  check_png(basn6a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, NULL, basn6a08_image_data);

  // 3x16 bits rgb color + 16 bit alpha-channel
  check_png(basn6a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, NULL, basn6a16_image_data);
}

static void test_png_suite_interlacing() {
  // black & white
  check_png(basi0g01_data, 32, 32, 1, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basi0g01_image_data);

  // 2 bit (4 level) grayscale
  check_png(basi0g02_data, 32, 32, 2, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basi0g02_image_data);

  // 4 bit (16 level) grayscale
  check_png(basi0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basi0g04_image_data);

  // 8 bit (256 level) grayscale
  check_png(basi0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basi0g08_image_data);

  // 16 bit (64k level) grayscale
  check_png(basi0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            basi0g16_image_data);

  // 3x8 bits rgb color
  check_png(basi2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            basi2c08_image_data);

  // 3x16 bits rgb color
  check_png(basi2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, basi2c16_image_data);

  // 1 bit (2 color) paletted
  check_png(basi3p01_data, 32, 32, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basi3p01_palette_data, NULL, basi3p01_image_data);

  // 2 bit (4 color) paletted
  check_png(basi3p02_data, 32, 32, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basi3p02_palette_data, NULL, basi3p02_image_data);

  // 4 bit (16 color) paletted
  check_png(basi3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basi3p04_palette_data, NULL, basi3p04_image_data);

  // 8 bit (256 color) paletted
  check_png(basi3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            basi3p08_palette_data, NULL, basi3p08_image_data);

  // 8 bit grayscale + 8 bit alpha-channel
  check_png(basi4a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, NULL, basi4a08_image_data);

  // 16 bit grayscale + 16 bit alpha-channel
  check_png(basi4a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, NULL, basi4a16_image_data);

  // 3x8 bits rgb color + 8 bit alpha-channel
  check_png(basi6a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, NULL, basi6a08_image_data);

  // 3x16 bits rgb color + 16 bit alpha-channel
  check_png(basi6a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, NULL, basi6a16_image_data);
}

static void test_png_suite_odd_sizes() {
  // 1x1 paletted file, interlaced
  check_png(s01i3p01_data, 1, 1, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s01i3p01_palette_data, NULL, s01i3p01_image_data);

  // 1x1 paletted file, no interlacing
  check_png(s01n3p01_data, 1, 1, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s01n3p01_palette_data, NULL, s01n3p01_image_data);

  // 2x2 paletted file, interlaced
  check_png(s02i3p01_data, 2, 2, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s02i3p01_palette_data, NULL, s02i3p01_image_data);

  // 2x2 paletted file, no interlacing
  check_png(s02n3p01_data, 2, 2, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s02n3p01_palette_data, NULL, s02n3p01_image_data);

  // 3x3 paletted file, interlaced
  check_png(s03i3p01_data, 3, 3, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s03i3p01_palette_data, NULL, s03i3p01_image_data);

  // 3x3 paletted file, no interlacing
  check_png(s03n3p01_data, 3, 3, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s03n3p01_palette_data, NULL, s03n3p01_image_data);

  // 4x4 paletted file, interlaced
  check_png(s04i3p01_data, 4, 4, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s04i3p01_palette_data, NULL, s04i3p01_image_data);

  // 4x4 paletted file, no interlacing
  check_png(s04n3p01_data, 4, 4, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s04n3p01_palette_data, NULL, s04n3p01_image_data);

  // 5x5 paletted file, interlaced
  check_png(s05i3p02_data, 5, 5, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s05i3p02_palette_data, NULL, s05i3p02_image_data);

  // 5x5 paletted file, no interlacing
  check_png(s05n3p02_data, 5, 5, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s05n3p02_palette_data, NULL, s05n3p02_image_data);

  // 6x6 paletted file, interlaced
  check_png(s06i3p02_data, 6, 6, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s06i3p02_palette_data, NULL, s06i3p02_image_data);

  // 6x6 paletted file, no interlacing
  check_png(s06n3p02_data, 6, 6, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s06n3p02_palette_data, NULL, s06n3p02_image_data);

  // 7x7 paletted file, interlaced
  check_png(s07i3p02_data, 7, 7, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s07i3p02_palette_data, NULL, s07i3p02_image_data);

  // 7x7 paletted file, no interlacing
  check_png(s07n3p02_data, 7, 7, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s07n3p02_palette_data, NULL, s07n3p02_image_data);

  // 8x8 paletted file, interlaced
  check_png(s08i3p02_data, 8, 8, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s08i3p02_palette_data, NULL, s08i3p02_image_data);

  // 8x8 paletted file, no interlacing
  check_png(s08n3p02_data, 8, 8, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s08n3p02_palette_data, NULL, s08n3p02_image_data);

  // 9x9 paletted file, interlaced
  check_png(s09i3p02_data, 9, 9, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s09i3p02_palette_data, NULL, s09i3p02_image_data);

  // 9x9 paletted file, no interlacing
  check_png(s09n3p02_data, 9, 9, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s09n3p02_palette_data, NULL, s09n3p02_image_data);

  // 32x32 paletted file, interlaced
  check_png(s32i3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s32i3p04_palette_data, NULL, s32i3p04_image_data);

  // 32x32 paletted file, no interlacing
  check_png(s32n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s32n3p04_palette_data, NULL, s32n3p04_image_data);

  // 33x33 paletted file, interlaced
  check_png(s33i3p04_data, 33, 33, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s33i3p04_palette_data, NULL, s33i3p04_image_data);

  // 33x33 paletted file, no interlacing
  check_png(s33n3p04_data, 33, 33, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s33n3p04_palette_data, NULL, s33n3p04_image_data);

  // 34x34 paletted file, interlaced
  check_png(s34i3p04_data, 34, 34, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s34i3p04_palette_data, NULL, s34i3p04_image_data);

  // 34x34 paletted file, no interlacing
  check_png(s34n3p04_data, 34, 34, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s34n3p04_palette_data, NULL, s34n3p04_image_data);

  // 35x35 paletted file, interlaced
  check_png(s35i3p04_data, 35, 35, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s35i3p04_palette_data, NULL, s35i3p04_image_data);

  // 35x35 paletted file, no interlacing
  check_png(s35n3p04_data, 35, 35, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s35n3p04_palette_data, NULL, s35n3p04_image_data);

  // 36x36 paletted file, interlaced
  check_png(s36i3p04_data, 36, 36, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s36i3p04_palette_data, NULL, s36i3p04_image_data);

  // 36x36 paletted file, no interlacing
  check_png(s36n3p04_data, 36, 36, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s36n3p04_palette_data, NULL, s36n3p04_image_data);

  // 37x37 paletted file, interlaced
  check_png(s37i3p04_data, 37, 37, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s37i3p04_palette_data, NULL, s37i3p04_image_data);

  // 37x37 paletted file, no interlacing
  check_png(s37n3p04_data, 37, 37, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s37n3p04_palette_data, NULL, s37n3p04_image_data);

  // 38x38 paletted file, interlaced
  check_png(s38i3p04_data, 38, 38, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s38i3p04_palette_data, NULL, s38i3p04_image_data);

  // 38x38 paletted file, no interlacing
  check_png(s38n3p04_data, 38, 38, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s38n3p04_palette_data, NULL, s38n3p04_image_data);

  // 39x39 paletted file, interlaced
  check_png(s39i3p04_data, 39, 39, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s39i3p04_palette_data, NULL, s39i3p04_image_data);

  // 39x39 paletted file, no interlacing
  check_png(s39n3p04_data, 39, 39, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s39n3p04_palette_data, NULL, s39n3p04_image_data);

  // 40x40 paletted file, interlaced
  check_png(s40i3p04_data, 40, 40, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s40i3p04_palette_data, NULL, s40i3p04_image_data);

  // 40x40 paletted file, no interlacing
  check_png(s40n3p04_data, 40, 40, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            s40n3p04_palette_data, NULL, s40n3p04_image_data);
}

static void test_png_suite_background_colours() {
  // 8 bit grayscale, alpha, no background chunk, interlaced
  check_png(bgai4a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, bgai4a08_background, bgai4a08_image_data);

  // 16 bit grayscale, alpha, no background chunk, interlaced
  check_png(bgai4a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, bgai4a16_background, bgai4a16_image_data);

  // 3x8 bits rgb color, alpha, no background chunk
  check_png(bgan6a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, bgan6a08_background, bgan6a08_image_data);

  // 3x16 bits rgb color, alpha, no background chunk
  check_png(bgan6a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, bgan6a16_background, bgan6a16_image_data);

  // 8 bit grayscale, alpha, black background chunk
  check_png(bgbn4a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, bgbn4a08_background, bgbn4a08_image_data);

  // 16 bit grayscale, alpha, gray background chunk
  check_png(bggn4a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA,
            NULL, bggn4a16_background, bggn4a16_image_data);

  // 3x8 bits rgb color, alpha, white background chunk
  check_png(bgwn6a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, bgwn6a08_background, bgwn6a08_image_data);

  // 3x16 bits rgb color, alpha, yellow background chunk
  check_png(bgyn6a16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            NULL, bgyn6a16_background, bgyn6a16_image_data);
}

static void test_png_suite_transparency() {
  // FIXME: Check transparency values

  // transparent, black background chunk
  check_png(tbbn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL,
            tbbn0g04_background, tbbn0g04_image_data);

  // transparent, blue background chunk
  check_png(tbbn2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            tbbn2c16_background, tbbn2c16_image_data);

  // transparent, black background chunk
  check_png(tbbn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tbbn3p08_palette_data, tbbn3p08_background, tbbn3p08_image_data);

  // transparent, green background chunk
  check_png(tbgn2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            tbgn2c16_background, tbgn2c16_image_data);

  // transparent, light-gray background chunk
  check_png(tbgn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tbgn3p08_palette_data, tbgn3p08_background, tbgn3p08_image_data);

  // transparent, red background chunk
  check_png(tbrn2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            tbrn2c08_background, tbrn2c08_image_data);

  // transparent, white background chunk
  check_png(tbwn0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL,
            tbwn0g16_background, tbwn0g16_image_data);

  // transparent, white background chunk
  check_png(tbwn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tbwn3p08_palette_data, tbwn3p08_background, tbwn3p08_image_data);

  // transparent, yellow background chunk
  check_png(tbyn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tbyn3p08_palette_data, tbyn3p08_background, tbyn3p08_image_data);

  // not transparent for reference (logo on gray)
  check_png(tp0n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            tp0n0g08_image_data);

  // not transparent for reference (logo on gray)
  check_png(tp0n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            tp0n2c08_image_data);

  // not transparent for reference (logo on gray)
  check_png(tp0n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tp0n3p08_palette_data, NULL, tp0n3p08_image_data);

  // transparent, but no background chunk
  check_png(tp1n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tp1n3p08_palette_data, NULL, tp1n3p08_image_data);

  // multiple levels of transparency, 3 entries
  check_png(tm3n3p02_data, 32, 32, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            tm3n3p02_palette_data, NULL, tm3n3p02_image_data);
}

static void test_png_suite_gamma_values() {
  // FIXME: Check gamma values

  // grayscale, file-gamma = 0.35
  check_png(g03n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g03n0g16_image_data);

  // color, file-gamma = 0.35
  check_png(g03n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g03n2c08_image_data);

  // paletted, file-gamma = 0.35
  check_png(g03n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g03n3p04_palette_data, NULL, g03n3p04_image_data);

  // grayscale, file-gamma = 0.45
  check_png(g04n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g04n0g16_image_data);

  // color, file-gamma = 0.45
  check_png(g04n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g04n2c08_image_data);

  // paletted, file-gamma = 0.45
  check_png(g04n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g04n3p04_palette_data, NULL, g04n3p04_image_data);

  // grayscale, file-gamma = 0.55
  check_png(g05n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g05n0g16_image_data);

  // color, file-gamma = 0.55
  check_png(g05n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g05n2c08_image_data);

  // paletted, file-gamma = 0.55
  check_png(g05n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g05n3p04_palette_data, NULL, g05n3p04_image_data);

  // grayscale, file-gamma = 0.70
  check_png(g07n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g07n0g16_image_data);

  // color, file-gamma = 0.70
  check_png(g07n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g07n2c08_image_data);

  // paletted, file-gamma = 0.70
  check_png(g07n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g07n3p04_palette_data, NULL, g07n3p04_image_data);

  // grayscale, file-gamma = 1.00
  check_png(g10n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g10n0g16_image_data);

  // color, file-gamma = 1.00
  check_png(g10n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g10n2c08_image_data);

  // paletted, file-gamma = 1.00
  check_png(g10n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g10n3p04_palette_data, NULL, g10n3p04_image_data);

  // grayscale, file-gamma = 2.50
  check_png(g25n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            g25n0g16_image_data);

  // color, file-gamma = 2.50
  check_png(g25n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            g25n2c08_image_data);

  // paletted, file-gamma = 2.50
  check_png(g25n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            g25n3p04_palette_data, NULL, g25n3p04_image_data);
}

static void test_png_suite_image_filtering() {
  // grayscale, no interlacing, filter-type 0
  check_png(f00n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f00n0g08_image_data);

  // color, no interlacing, filter-type 0
  check_png(f00n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            f00n2c08_image_data);

  // grayscale, no interlacing, filter-type 1
  check_png(f01n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f01n0g08_image_data);

  // color, no interlacing, filter-type 1
  check_png(f01n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            f01n2c08_image_data);

  // grayscale, no interlacing, filter-type 2
  check_png(f02n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f02n0g08_image_data);

  // color, no interlacing, filter-type 2
  check_png(f02n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            f02n2c08_image_data);

  // grayscale, no interlacing, filter-type 3
  check_png(f03n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f03n0g08_image_data);

  // color, no interlacing, filter-type 3
  check_png(f03n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            f03n2c08_image_data);

  // grayscale, no interlacing, filter-type 4
  check_png(f04n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f04n0g08_image_data);

  // color, no interlacing, filter-type 4
  check_png(f04n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            f04n2c08_image_data);

  // bit-depth 4, filter changing per scanline
  check_png(f99n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            f99n0g04_image_data);
}

static void test_png_suite_additional_palettes() {
  // FIXME: Check suggested palette.

  // six-cube palette-chunk in true-color image
  check_png(pp0n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR,
            pp0n2c16_palette_data, NULL, pp0n2c16_image_data);

  // six-cube palette-chunk in true-color+alpha image
  check_png(pp0n6a08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA,
            pp0n6a08_palette_data, NULL, pp0n6a08_image_data);

  // six-cube suggested palette (1 byte) in grayscale image
  check_png(ps1n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            ps1n0g08_image_data);

  // six-cube suggested palette (1 byte) in true-color image
  check_png(ps1n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, ps1n2c16_image_data);

  // six-cube suggested palette (2 bytes) in grayscale image
  check_png(ps2n0g08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            ps2n0g08_image_data);

  // six-cube suggested palette (2 bytes) in true-color image
  check_png(ps2n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, ps2n2c16_image_data);
}

static void test_png_suite_ancillary_chunks() {
  // FIXME: Check ancillary data.

  // chroma chunk w:0.3127,0.3290 r:0.64,0.33 g:0.30,0.60 b:0.15,0.06
  check_png(ccwn2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            ccwn2c08_image_data);

  // chroma chunk w:0.3127,0.3290 r:0.64,0.33 g:0.30,0.60 b:0.15,0.06
  check_png(ccwn3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            ccwn3p08_palette_data, NULL, ccwn3p08_image_data);

  // physical pixel dimensions, 8x32 flat pixels
  check_png(cdfn2c08_data, 8, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cdfn2c08_image_data);

  // physical pixel dimensions, 32x8 high pixels
  check_png(cdhn2c08_data, 32, 8, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cdhn2c08_image_data);

  // physical pixel dimensions, 8x8 square pixels
  check_png(cdsn2c08_data, 8, 8, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cdsn2c08_image_data);

  // physical pixel dimensions, 1000 pixels per 1 meter
  check_png(cdun2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cdun2c08_image_data);

  // histogram 15 colors
  check_png(ch1n3p04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            ch1n3p04_palette_data, NULL, ch1n3p04_image_data);

  // histogram 256 colors
  check_png(ch2n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            ch2n3p08_palette_data, NULL, ch2n3p08_image_data);

  // modification time, 01-jan-2000 12:34:56
  check_png(cm0n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            cm0n0g04_image_data);

  // modification time, 01-jan-1970 00:00:00
  check_png(cm7n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            cm7n0g04_image_data);

  // modification time, 31-dec-1999 23:59:59
  check_png(cm9n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            cm9n0g04_image_data);

  // color, 13 significant bits
  check_png(cs3n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, cs3n2c16_image_data);

  // paletted, 3 significant bits
  check_png(cs3n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            cs3n3p08_palette_data, NULL, cs3n3p08_image_data);

  // color, 5 significant bits
  check_png(cs5n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cs5n2c08_image_data);

  // paletted, 5 significant bits
  check_png(cs5n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            cs5n3p08_palette_data, NULL, cs5n3p08_image_data);

  // color, 8 significant bits (reference)
  check_png(cs8n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            cs8n2c08_image_data);

  // paletted, 8 significant bits (reference)
  check_png(cs8n3p08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR,
            cs8n3p08_palette_data, NULL, cs8n3p08_image_data);

  // no textual data
  check_png(ct0n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            ct0n0g04_image_data);

  // with textual data
  UtObjectRef text_image =
      check_png_full(ct1n0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, ct1n0g04_image_data);
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Title"),
                          "PngSuite");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Author"),
                          "Willem A.J. van Schaik\n"
                          "(willem@schaik.com)");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Copyright"),
                          "Copyright Willem van Schaik, Singapore 1995-96");
  ut_assert_cstring_equal(
      ut_png_image_get_text(text_image, "Description"),
      "A compilation of a set of images created to test the\n"
      "various color-types of the PNG format. Included are\n"
      "black&white, color, paletted, with alpha channel, with\n"
      "transparency formats. All bit-depths allowed according\n"
      "to the spec are present.");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Software"),
                          "Created on a NeXTstation color using \"pnmtopng\".");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Disclaimer"),
                          "Freeware.");

  // with compressed textual data
  UtObjectRef compressed_text_image =
      check_png_full(ctzn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, ctzn0g04_image_data);
  // The following text was uncompressed:
  ut_assert_cstring_equal(ut_png_image_get_text(compressed_text_image, "Title"),
                          "PngSuite");
  ut_assert_cstring_equal(
      ut_png_image_get_text(compressed_text_image, "Author"),
      "Willem A.J. van Schaik\n"
      "(willem@schaik.com)");
  // The following text was compressed:
  ut_assert_cstring_equal(
      ut_png_image_get_text(compressed_text_image, "Copyright"),
      "Copyright Willem van Schaik, Singapore 1995-96");
  ut_assert_cstring_equal(
      ut_png_image_get_text(compressed_text_image, "Description"),
      "A compilation of a set of images created to test the\n"
      "various color-types of the PNG format. Included are\n"
      "black&white, color, paletted, with alpha channel, with\n"
      "transparency formats. All bit-depths allowed according\n"
      "to the spec are present.");
  ut_assert_cstring_equal(
      ut_png_image_get_text(compressed_text_image, "Software"),
      "Created on a NeXTstation color using \"pnmtopng\".");
  ut_assert_cstring_equal(
      ut_png_image_get_text(compressed_text_image, "Disclaimer"), "Freeware.");

  // international UTF-8, english
  UtObjectRef en_text_image =
      check_png_full(cten0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, cten0g04_image_data);
  const char *translated_keyword;
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Title", "en",
                                          &translated_keyword),
      "PngSuite");
  ut_assert_cstring_equal(translated_keyword, "Title");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Author", "en",
                                          &translated_keyword),
      "Willem van Schaik (willem@schaik.com)");
  ut_assert_cstring_equal(translated_keyword, "Author");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Copyright", "en",
                                          &translated_keyword),
      "Copyright Willem van Schaik, Canada 2011");
  ut_assert_cstring_equal(translated_keyword, "Copyright");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Description", "en",
                                          &translated_keyword),
      "A compilation of a set of images created to test the various "
      "color-types of the PNG format. Included are black&white, color, "
      "paletted, with alpha channel, with transparency formats. All bit-depths "
      "allowed according to the spec are present.");
  ut_assert_cstring_equal(translated_keyword, "Description");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Software", "en",
                                          &translated_keyword),
      "Created on a NeXTstation color using \"pnmtopng\".");
  ut_assert_cstring_equal(translated_keyword, "Software");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(en_text_image, "Disclaimer", "en",
                                          &translated_keyword),
      "Freeware.");
  ut_assert_cstring_equal(translated_keyword, "Disclaimer");

  // international UTF-8, finnish
  UtObjectRef fi_text_image =
      check_png_full(ctfn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, ctfn0g04_image_data);
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Title", "fi",
                                          &translated_keyword),
      "PngSuite");
  ut_assert_cstring_equal(translated_keyword, "Otsikko");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Author", "fi",
                                          &translated_keyword),
      "Willem van Schaik (willem@schaik.com)");
  ut_assert_cstring_equal(translated_keyword, "Tekijä");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Copyright", "fi",
                                          &translated_keyword),
      "Copyright Willem van Schaik, Kanada 2011");
  ut_assert_cstring_equal(translated_keyword, "Tekijänoikeudet");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Description", "fi",
                                          &translated_keyword),
      "kokoelma joukon kuvia luotu testata eri väri-tyyppisiä PNG-muodossa. "
      "Mukana on mustavalkoinen, väri, paletted, alpha-kanava, avoimuuden "
      "muodossa. Kaikki bit-syvyydessä mukaan sallittua spec on "
      "​​läsnä.");
  ut_assert_cstring_equal(translated_keyword, "Kuvaus");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Software", "fi",
                                          &translated_keyword),
      "Luotu NeXTstation väriä \"pnmtopng\".");
  ut_assert_cstring_equal(translated_keyword, "Ohjelmistot");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(fi_text_image, "Disclaimer", "fi",
                                          &translated_keyword),
      "Freeware.");
  ut_assert_cstring_equal(translated_keyword, "Vastuuvapauslauseke");

  // international UTF-8, greek
  UtObjectRef el_text_image =
      check_png_full(ctgn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, ctgn0g04_image_data);
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Title", "el",
                                          &translated_keyword),
      "PngSuite");
  ut_assert_cstring_equal(translated_keyword, "Τίτλος");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Author", "el",
                                          &translated_keyword),
      "Willem van Schaik (willem@schaik.com)");
  ut_assert_cstring_equal(translated_keyword, "Συγγραφέας");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Copyright", "el",
                                          &translated_keyword),
      "Πνευματικά δικαιώματα Schaik van Willem, Καναδάς 2011");
  ut_assert_cstring_equal(translated_keyword, "Πνευματικά δικαιώματα");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Description", "el",
                                          &translated_keyword),
      "Μια συλλογή από ένα σύνολο εικόνων που δημιουργήθηκαν για τη δοκιμή των "
      "διαφόρων χρωμάτων-τύπων του μορφή PNG. Περιλαμβάνονται οι ασπρόμαυρες, "
      "χρώμα, paletted, με άλφα κανάλι, με μορφές της διαφάνειας. Όλοι "
      "λίγο-βάθη επιτρέπεται σύμφωνα με το spec είναι παρόντες.");
  ut_assert_cstring_equal(translated_keyword, "Περιγραφή");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Software", "el",
                                          &translated_keyword),
      "Δημιουργήθηκε σε ένα χρώμα NeXTstation χρησιμοποιώντας \"pnmtopng\".");
  ut_assert_cstring_equal(translated_keyword, "Λογισμικό");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(el_text_image, "Disclaimer", "el",
                                          &translated_keyword),
      "Δωρεάν λογισμικό.");
  ut_assert_cstring_equal(translated_keyword, "Αποποίηση");

  // international UTF-8, hindi
  UtObjectRef hi_text_image =
      check_png_full(cthn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, cthn0g04_image_data);
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Title", "hi",
                                          &translated_keyword),
      "PngSuite");
  ut_assert_cstring_equal(translated_keyword, "शीर्षक");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Author", "hi",
                                          &translated_keyword),
      "Willem van Schaik (willem@schaik.com)");
  ut_assert_cstring_equal(translated_keyword, "लेखक");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Copyright", "hi",
                                          &translated_keyword),
      "कॉपीराइट Willem van Schaik, 2011 कनाडा");
  ut_assert_cstring_equal(translated_keyword, "कॉपीराइट");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Description", "hi",
                                          &translated_keyword),
      "करने के लिए PNG प्रारूप के विभिन्न रंग प्रकार परीक्षण बनाया छवियों का एक सेट का एक "
      "संकलन. शामिल काले और सफेद, रंग, पैलेटेड हैं, अल्फा चैनल के साथ पारदर्शिता स्वरूपों के "
      "साथ. सभी बिट गहराई कल्पना के अनुसार की अनुमति दी मौजूद हैं.");
  ut_assert_cstring_equal(translated_keyword, "विवरण");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Software", "hi",
                                          &translated_keyword),
      "एक NeXTstation \"pnmtopng 'का उपयोग कर रंग पर बनाया गया.");
  ut_assert_cstring_equal(translated_keyword, "सॉफ्टवेयर");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(hi_text_image, "Disclaimer", "hi",
                                          &translated_keyword),
      "फ्रीवेयर.");
  ut_assert_cstring_equal(translated_keyword, "अस्वीकरण");

  // international UTF-8, japanese
  UtObjectRef ja_text_image =
      check_png_full(ctjn0g04_data, 32, 32, 4, UT_PNG_COLOUR_TYPE_GREYSCALE,
                     NULL, NULL, ctjn0g04_image_data);
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Title", "ja",
                                          &translated_keyword),
      "PngSuite");
  ut_assert_cstring_equal(translated_keyword, "タイトル");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Author", "ja",
                                          &translated_keyword),
      "Willem van Schaik (willem@schaik.com)");
  ut_assert_cstring_equal(translated_keyword, "著者");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Copyright", "ja",
                                          &translated_keyword),
      "著作権ウィレムヴァンシャイク、カナダ2011");
  ut_assert_cstring_equal(translated_keyword, "本文へ");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Description", "ja",
                                          &translated_keyword),
      "PNG形式の様々な色の種類をテストするために作成されたイメージのセットのコ"
      "ンパイル。含まれているのは透明度のフォーマットで、アルファチャネルを持つ"
      "、白黒、カラー、パレットです。すべてのビット深度が存在している仕様に従っ"
      "たことができました。");
  ut_assert_cstring_equal(translated_keyword, "概要");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Software", "ja",
                                          &translated_keyword),
      "\"pnmtopng\"を使用してNeXTstation色上に作成されます。");
  ut_assert_cstring_equal(translated_keyword, "ソフトウェア");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(ja_text_image, "Disclaimer", "ja",
                                          &translated_keyword),
      "フリーウェア。");
  ut_assert_cstring_equal(translated_keyword, "免責事項");

  // exif - image attributes
  check_png(exif2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            exif2c08_image_data);
}

static void test_png_suite_chunk_ordering() {
  // grayscale mother image with 1 idat-chunk
  check_png(oi1n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            oi1n0g16_image_data);

  // color mother image with 1 idat-chunk
  check_png(oi1n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, oi1n2c16_image_data);

  // grayscale image with 2 idat-chunks
  check_png(oi2n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            oi2n0g16_image_data);

  // color image with 2 idat-chunks
  check_png(oi2n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, oi2n2c16_image_data);

  // grayscale image with 4 unequal sized idat-chunks
  check_png(oi4n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            oi4n0g16_image_data);

  // color image with 4 unequal sized idat-chunks
  check_png(oi4n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, oi4n2c16_image_data);

  // grayscale image with all idat-chunks length one
  check_png(oi9n0g16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, NULL, NULL,
            oi9n0g16_image_data);

  // color image with all idat-chunks length one
  check_png(oi9n2c16_data, 32, 32, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL,
            NULL, oi9n2c16_image_data);
}

static void test_png_suite_zlib_compression_level() {
  // color, no interlacing, compression level 0 (none)
  check_png(z00n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            z00n2c08_image_data);

  // color, no interlacing, compression level 3
  check_png(z03n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            z03n2c08_image_data);

  // color, no interlacing, compression level 6 (default)
  check_png(z06n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            z06n2c08_image_data);

  // color, no interlacing, compression level 9
  check_png(z09n2c08_data, 32, 32, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, NULL, NULL,
            z09n2c08_image_data);
}

static void test_png_suite_corrupted_files() {
  // signature byte 1 MSBit reset to zero
  check_corrupt_png(xs1n0g01_data);

  // signature byte 2 is a 'Q'
  check_corrupt_png(xs2n0g01_data);

  // signature byte 4 lowercase
  check_corrupt_png(xs4n0g01_data);

  // 7th byte a space instead of control-Z
  check_corrupt_png(xs7n0g01_data);

  // added cr bytes
  check_corrupt_png(xcrn0g04_data);

  // added lf bytes
  check_corrupt_png(xlfn0g04_data);

  // incorrect IHDR checksum
  check_corrupt_png(xhdn0g08_data);

  // color type 1
  check_corrupt_png(xc1n0g08_data);

  // color type 9
  check_corrupt_png(xc9n2c08_data);

  // bit-depth 0
  check_corrupt_png(xd0n2c08_data);

  // bit-depth 3
  check_corrupt_png(xd3n2c08_data);

  // bit-depth 99
  check_corrupt_png(xd9n2c08_data);

  // missing IDAT chunk
  check_corrupt_png(xdtn0g01_data);

  // incorrect IDAT checksum
  check_corrupt_png(xcsn0g01_data);
}

int main(int argc, char **argv) {
  test_png_suite_basic_formats();
  test_png_suite_interlacing();
  test_png_suite_odd_sizes();
  test_png_suite_background_colours();
  test_png_suite_transparency();
  test_png_suite_gamma_values();
  test_png_suite_image_filtering();
  test_png_suite_additional_palettes();
  test_png_suite_ancillary_chunks();
  test_png_suite_chunk_ordering();
  test_png_suite_zlib_compression_level();
  test_png_suite_corrupted_files();

  return 0;
}
