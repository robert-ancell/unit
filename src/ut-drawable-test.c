#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef clear_buffer = ut_rgba8888_buffer_new(1, 1);
  UtObjectRef clear_color = ut_color_new_from_hex_string("#77216f");
  ut_drawable_clear(clear_buffer, clear_color);
  uint8_t r, g, b, a;
  ut_rgba8888_buffer_get_pixel(clear_buffer, 0, 0, &r, &g, &b, &a);
  ut_assert_int_equal(r, 0x77);
  ut_assert_int_equal(g, 0x21);
  ut_assert_int_equal(b, 0x6f);
  ut_assert_int_equal(a, 0xff);

  return 0;
}
