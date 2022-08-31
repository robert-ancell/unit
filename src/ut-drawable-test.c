#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  UtObjectRef buffer = ut_rgba8888_buffer_new(100, 100);

  UtObjectRef bg_color = ut_color_new_from_hex_string("#77216f");
  ut_drawable_clear(buffer, bg_color);
  UtObjectRef fg_color = ut_color_new_from_hex_string("#e95420");
  ut_drawable_render_box(buffer, 25, 25, 50, 50, fg_color);

  size_t width = ut_image_buffer_get_width(buffer);
  size_t height = ut_image_buffer_get_height(buffer);
  UtObjectRef ppm = ut_string_new("");
  ut_string_append(ppm, "P3\n");
  ut_string_append_printf(ppm, "%zi %zi\n", width, height);
  ut_string_append(ppm, "255\n");
  UtObject *data = ut_image_buffer_get_data(buffer);
  uint8_t *d = ut_uint8_array_get_data(data);
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      ut_string_append_printf(ppm, "%d %d %d\n", d[0], d[1], d[2]);
      d += 4;
    }
  }
  printf("%s\n", ut_string_get_text(ppm));

  return 0;
}
