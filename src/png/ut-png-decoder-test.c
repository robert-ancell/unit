#include <assert.h>
#include <stdio.h>

#include "ut-png-decoder-test-data.h"
#include "ut.h"

// Check the PNG image in [hex_data] can be decoded and matches the expected
// properties.
static void check_png(const char *hex_data, uint32_t width, uint32_t height,
                      uint8_t bit_depth, UtPngColourType colour_type,
                      const char *hex_palette_data, const char *hex_background,
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
    assert(ut_png_image_get_background_colour(image) == NULL);
  } else {
    assert(ut_png_image_get_background_colour(image) != NULL);
    ut_assert_uint8_list_equal_hex(ut_png_image_get_background_colour(image),
                                   hex_background);
  }
  if (hex_palette_data == NULL) {
    assert(ut_png_image_get_palette(image) == NULL);
  } else {
    assert(ut_png_image_get_palette(image) != NULL);
    ut_assert_uint8_list_equal_hex(ut_png_image_get_palette(image),
                                   hex_palette_data);
  }
  ut_assert_uint8_list_equal_hex(ut_png_image_get_data(image), hex_image_data);
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
  // FIXME
}

static void test_png_suite_gamma_values() {
  // FIXME
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
  // FIXME
}

static void test_png_suite_ancillary_chunks() {
  // FIXME
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
  // FIXME
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
