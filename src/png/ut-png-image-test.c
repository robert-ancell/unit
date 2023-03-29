#include <assert.h>

#include "ut.h"

int main(int argc, char **argv) {
  // Image:
  // 01
  // 10
  UtObjectRef g1_image_data = ut_uint8_list_new_from_hex_string("40"
                                                                "80");
  UtObjectRef g1_image =
      ut_png_image_new(2, 2, 1, UT_PNG_COLOUR_TYPE_GREYSCALE, g1_image_data);
  UtObjectRef g1_rgba = ut_png_image_to_rgba(g1_image);
  ut_assert_uint8_list_equal_hex(g1_rgba, "000000ffffffffff"
                                          "ffffffff000000ff");

  // Image:
  // 01
  // 23
  UtObjectRef g2_image_data = ut_uint8_list_new_from_hex_string("10"
                                                                "b0");
  UtObjectRef g2_image =
      ut_png_image_new(2, 2, 2, UT_PNG_COLOUR_TYPE_GREYSCALE, g2_image_data);
  UtObjectRef g2_rgba = ut_png_image_to_rgba(g2_image);
  ut_assert_uint8_list_equal_hex(g2_rgba, "000000ff555555ff"
                                          "aaaaaaffffffffff");

  UtObjectRef g4_image_data = ut_uint8_list_new_from_hex_string("0123"
                                                                "4567"
                                                                "89ab"
                                                                "cdef");
  UtObjectRef g4_image =
      ut_png_image_new(4, 4, 4, UT_PNG_COLOUR_TYPE_GREYSCALE, g4_image_data);
  UtObjectRef g4_rgba = ut_png_image_to_rgba(g4_image);
  ut_assert_uint8_list_equal_hex(g4_rgba, "000000ff111111ff222222ff333333ff"
                                          "444444ff555555ff666666ff777777ff"
                                          "888888ff999999ffaaaaaaffbbbbbbff"
                                          "ccccccffddddddffeeeeeeffffffffff");

  UtObjectRef g8_image_data = ut_uint8_list_new_from_hex_string("00112233"
                                                                "44556677"
                                                                "8899aabb"
                                                                "ccddeeff");
  UtObjectRef g8_image =
      ut_png_image_new(4, 4, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, g8_image_data);
  UtObjectRef g8_rgba = ut_png_image_to_rgba(g8_image);
  ut_assert_uint8_list_equal_hex(g8_rgba, "000000ff111111ff222222ff333333ff"
                                          "444444ff555555ff666666ff777777ff"
                                          "888888ff999999ffaaaaaaffbbbbbbff"
                                          "ccccccffddddddffeeeeeeffffffffff");

  UtObjectRef g16_image_data =
      ut_uint8_list_new_from_hex_string("0000111122223333"
                                        "4444555566667777"
                                        "88889999aaaabbbb"
                                        "ccccddddeeeeffff");
  UtObjectRef g16_image =
      ut_png_image_new(4, 4, 16, UT_PNG_COLOUR_TYPE_GREYSCALE, g16_image_data);
  UtObjectRef g16_rgba = ut_png_image_to_rgba(g16_image);
  ut_assert_uint8_list_equal_hex(g16_rgba, "000000ff111111ff222222ff333333ff"
                                           "444444ff555555ff666666ff777777ff"
                                           "888888ff999999ffaaaaaaffbbbbbbff"
                                           "ccccccffddddddffeeeeeeffffffffff");

  UtObjectRef t8_image_data = ut_uint8_list_new_from_hex_string("112233ddeeff"
                                                                "aabbcc445566");
  UtObjectRef t8_image =
      ut_png_image_new(2, 2, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR, t8_image_data);
  UtObjectRef t8_rgba = ut_png_image_to_rgba(t8_image);
  ut_assert_uint8_list_equal_hex(t8_rgba, "112233ffddeeffff"
                                          "aabbccff445566ff");

  UtObjectRef t16_image_data =
      ut_uint8_list_new_from_hex_string("111122223333ddddeeeeffff"
                                        "aaaabbbbcccc444455556666");
  UtObjectRef t16_image =
      ut_png_image_new(2, 2, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR, t16_image_data);
  UtObjectRef t16_rgba = ut_png_image_to_rgba(t16_image);
  ut_assert_uint8_list_equal_hex(t16_rgba, "112233ffddeeffff"
                                           "aabbccff445566ff");

  // Image:
  // 01
  // 10
  UtObjectRef i1_image_data = ut_uint8_list_new_from_hex_string("40"
                                                                "80");
  UtObjectRef i1_image_palette = ut_uint8_list_new_from_hex_string("112233"
                                                                   "ddeeff");
  UtObjectRef i1_image = ut_png_image_new(
      2, 2, 1, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR, i1_image_data);
  ut_png_image_set_palette(i1_image, i1_image_palette);
  UtObjectRef i1_rgba = ut_png_image_to_rgba(i1_image);
  ut_assert_uint8_list_equal_hex(i1_rgba, "112233ffddeeffff"
                                          "ddeeffff112233ff");

  // Image:
  // 01
  // 23
  UtObjectRef i2_image_data = ut_uint8_list_new_from_hex_string("10"
                                                                "b0");
  UtObjectRef i2_image_palette = ut_uint8_list_new_from_hex_string("112233"
                                                                   "445566"
                                                                   "aabbcc"
                                                                   "ddeeff");
  UtObjectRef i2_image = ut_png_image_new(
      2, 2, 2, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR, i2_image_data);
  ut_png_image_set_palette(i2_image, i2_image_palette);
  UtObjectRef i2_rgba = ut_png_image_to_rgba(i2_image);
  ut_assert_uint8_list_equal_hex(i2_rgba, "112233ff445566ff"
                                          "aabbccffddeeffff");

  UtObjectRef i4_image_data = ut_uint8_list_new_from_hex_string("0123"
                                                                "4567"
                                                                "89ab"
                                                                "cdef");
  UtObjectRef i4_image_palette = ut_uint8_list_new_from_hex_string("000000"
                                                                   "ff0000"
                                                                   "00ff00"
                                                                   "0000ff"
                                                                   "ffff00"
                                                                   "ff00ff"
                                                                   "00ffff"
                                                                   "ffffff"
                                                                   "555555"
                                                                   "aaaaaa"
                                                                   "7f0000"
                                                                   "007f00"
                                                                   "00007f"
                                                                   "7f7f00"
                                                                   "7f007f"
                                                                   "007f7f");
  UtObjectRef i4_image = ut_png_image_new(
      4, 4, 4, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR, i4_image_data);
  ut_png_image_set_palette(i4_image, i4_image_palette);
  UtObjectRef i4_rgba = ut_png_image_to_rgba(i4_image);
  ut_assert_uint8_list_equal_hex(i4_rgba, "000000ffff0000ff00ff00ff0000ffff"
                                          "ffff00ffff00ffff00ffffffffffffff"
                                          "555555ffaaaaaaff7f0000ff007f00ff"
                                          "00007fff7f7f00ff7f007fff007f7fff");

  UtObjectRef i8_image_data = ut_uint8_list_new_from_hex_string("00112233"
                                                                "44556677"
                                                                "8899aabb"
                                                                "ccddeeff");
  UtObjectRef i8_image_palette = ut_uint8_list_new_from_hex_string(
      "000000010000020000030000040000050000060000070000"
      "0800000900000a00000b00000c00000d00000e00000f0000"
      "100000110000120000130000140000150000160000170000"
      "1800001900001a00001b00001c00001d00001e00001f0000"
      "200000210000220000230000240000250000260000270000"
      "2800002900002a00002b00002c00002d00002e00002f0000"
      "300000310000320000330000340000350000360000370000"
      "3800003900003a00003b00003c00003d00003e00003f0000"
      "400000410000420000430000440000450000460000470000"
      "4800004900004a00004b00004c00004d00004e00004f0000"
      "500000510000520000530000540000550000560000570000"
      "5800005900005a00005b00005c00005d00005e00005f0000"
      "600000610000620000630000640000650000660000670000"
      "6800006900006a00006b00006c00006d00006e00006f0000"
      "700000710000720000730000740000750000760000770000"
      "7800007900007a00007b00007c00007d00007e00007f0000"
      "800000810000820000830000840000850000860000870000"
      "8800008900008a00008b00008c00008d00008e00008f0000"
      "900000910000920000930000940000950000960000970000"
      "9800009900009a00009b00009c00009d00009e00009f0000"
      "a00000a10000a20000a30000a40000a50000a60000a70000"
      "a80000a90000aa0000ab0000ac0000ad0000ae0000af0000"
      "b00000b10000b20000b30000b40000b50000b60000b70000"
      "b80000b90000ba0000bb0000bc0000bd0000be0000bf0000"
      "c00000c10000c20000c30000c40000c50000c60000c70000"
      "c80000c90000ca0000cb0000cc0000cd0000ce0000cf0000"
      "d00000d10000d20000d30000d40000d50000d60000d70000"
      "d80000d90000da0000db0000dc0000dd0000de0000df0000"
      "e00000e10000e20000e30000e40000e50000e60000e70000"
      "e80000e90000ea0000eb0000ec0000ed0000ee0000ef0000"
      "f00000f10000f20000f30000f40000f50000f60000f70000"
      "f80000f90000fa0000fb0000fc0000fd0000fe0000ff0000");
  UtObjectRef i8_image = ut_png_image_new(
      4, 4, 8, UT_PNG_COLOUR_TYPE_INDEXED_COLOUR, i8_image_data);
  ut_png_image_set_palette(i8_image, i8_image_palette);
  UtObjectRef i8_rgba = ut_png_image_to_rgba(i8_image);
  ut_assert_uint8_list_equal_hex(i8_rgba, "000000ff110000ff220000ff330000ff"
                                          "440000ff550000ff660000ff770000ff"
                                          "880000ff990000ffaa0000ffbb0000ff"
                                          "cc0000ffdd0000ffee0000ffff0000ff");

  UtObjectRef ga8_image_data = ut_uint8_list_new_from_hex_string("01002355"
                                                                 "cdaaefff");
  UtObjectRef ga8_image = ut_png_image_new(
      2, 2, 8, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA, ga8_image_data);
  UtObjectRef ga8_rgba = ut_png_image_to_rgba(ga8_image);
  ut_assert_uint8_list_equal_hex(ga8_rgba, "0101010023232355"
                                           "cdcdcdaaefefefff");

  UtObjectRef ga16_image_data =
      ut_uint8_list_new_from_hex_string("1111000022225555"
                                        "eeeeaaaaffffffff");
  UtObjectRef ga16_image = ut_png_image_new(
      2, 2, 16, UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA, ga16_image_data);
  UtObjectRef ga16_rgba = ut_png_image_to_rgba(ga16_image);
  ut_assert_uint8_list_equal_hex(ga16_rgba, "1111110022222255"
                                            "eeeeeeaaffffffff");

  UtObjectRef ta8_image_data =
      ut_uint8_list_new_from_hex_string("11223300ddeeff55"
                                        "aabbccaa445566ff");
  UtObjectRef ta8_image = ut_png_image_new(
      2, 2, 8, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA, ta8_image_data);
  UtObjectRef ta8_rgba = ut_png_image_to_rgba(ta8_image);
  ut_assert_uint8_list_equal_hex(ta8_rgba, "11223300ddeeff55"
                                           "aabbccaa445566ff");

  UtObjectRef ta16_image_data =
      ut_uint8_list_new_from_hex_string("1111222233330000ddddeeeeffff5555"
                                        "aaaabbbbccccaaaa444455556666ffff");
  UtObjectRef ta16_image = ut_png_image_new(
      2, 2, 16, UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA, ta16_image_data);
  UtObjectRef ta16_rgba = ut_png_image_to_rgba(ta16_image);
  ut_assert_uint8_list_equal_hex(ta16_rgba, "11223300ddeeff55"
                                            "aabbccaa445566ff");

  UtObjectRef text_image_data = ut_uint8_list_new_from_hex_string("00");
  UtObjectRef text_image =
      ut_png_image_new(1, 1, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, text_image_data);
  ut_png_image_set_text(text_image, "Title", "Test Text");
  ut_png_image_set_text(text_image, "Author", "Foo <foo@example.com>");
  ut_png_image_set_text(text_image, "Description", "Lorem Ipsum.\nFoo Bar");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Title"),
                          "Test Text");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Author"),
                          "Foo <foo@example.com>");
  ut_assert_cstring_equal(ut_png_image_get_text(text_image, "Description"),
                          "Lorem Ipsum.\nFoo Bar");
  assert(ut_png_image_get_text(text_image, "Comment") == NULL);

  UtObjectRef international_text_image_data =
      ut_uint8_list_new_from_hex_string("00");
  UtObjectRef international_text_image = ut_png_image_new(
      1, 1, 8, UT_PNG_COLOUR_TYPE_GREYSCALE, international_text_image_data);
  ut_png_image_set_international_text(international_text_image, "Title", "de",
                                      "Titel", "Testtext");
  ut_png_image_set_international_text(international_text_image, "Author", "DE",
                                      "Autor", "Foo <foo-de@example.com>");
  const char *translated_keyword;
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(international_text_image, "Title",
                                          "de", &translated_keyword),
      "Testtext");
  ut_assert_cstring_equal(translated_keyword, "Titel");
  ut_assert_cstring_equal(
      ut_png_image_get_international_text(international_text_image, "Author",
                                          "De", &translated_keyword),
      "Foo <foo-de@example.com>");
  ut_assert_cstring_equal(translated_keyword, "Autor");
  assert(ut_png_image_get_international_text(international_text_image, "fi",
                                             "Title",
                                             &translated_keyword) == NULL);
  assert(translated_keyword == NULL);

  return 0;
}
