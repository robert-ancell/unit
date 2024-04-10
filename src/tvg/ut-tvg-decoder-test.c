#include "ut.h"

// Check the TVG image in [hex_data] can be decoded and matches the expected
// properties.
static UtObject *check_tvg_full(const char *hex_data, uint32_t width,
                                uint32_t height) {
  UtObjectRef data = ut_uint8_list_new_from_hex_string(hex_data);
  UtObjectRef data_stream = ut_list_input_stream_new(data);
  UtObjectRef decoder = ut_tvg_decoder_new(data_stream);
  UtObjectRef image = ut_tvg_decoder_decode_sync(decoder);
  ut_assert_is_not_error(image);
  ut_assert_int_equal(ut_tvg_image_get_width(image), width);
  ut_assert_int_equal(ut_tvg_image_get_height(image), height);

  return ut_object_ref(image);
}

// Check the TVG image in [hex_data] can be decoded and matches the expected
// properties.
static void check_tvg(const char *hex_data, uint32_t width, uint32_t height) {
  UtObjectRef image = check_tvg_full(hex_data, width, height);
}

int main(int argc, char **argv) {
  check_tvg("7256014218180229adfffffff1e8ff0302000405033004000c14022c030c421b57"
            "305c0345575442542c0214454403404b38513054032851204b1b44031a42194018"
            "3e03183723323032033d324837483e0347404642454430140336143c193c20033c"
            "26372c302c032a2c2427242003241a2914301400",
            24, 24);

  return 0;
}
