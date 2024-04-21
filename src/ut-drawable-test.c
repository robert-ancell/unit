#include "ut.h"

static double parse_double(const char *text) { return strtod(text, NULL); }

static UtObject *parse_verticies(const char *text) {
  UtObject *verticies = ut_float32_list_new();
  UtObjectRef values = ut_cstring_split(text, ",");
  size_t values_length = ut_list_get_length(values);
  for (size_t i = 0; i < values_length; i++) {
    const char *value = ut_string_list_get_element(values, i);
    ut_float32_list_append(verticies, parse_double(value));
  }
  return verticies;
}

static uint16_t parse_uint16(const char *text) {
  return strtoul(text, NULL, 10);
}

static UtObject *parse_triangles(const char *text) {
  UtObject *triangles = ut_uint16_list_new();
  UtObjectRef values = ut_cstring_split(text, ",");
  size_t values_length = ut_list_get_length(values);
  for (size_t i = 0; i < values_length; i++) {
    const char *value = ut_string_list_get_element(values, i);
    ut_uint16_list_append(triangles, parse_uint16(value));
  }
  return triangles;
}

static char get_pixel_text(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  if (r == 0 && g == 0 && b == 0 && a == 0) {
    return ' ';
  } else if (r == 0xff && g == 0 && b == 0 && a == 0xff) {
    return 'R';
  } else if (r == 0 && g == 0xff && b == 0 && a == 0xff) {
    return 'G';
  } else if (r == 0 && g == 0 && b == 0xff && a == 0xff) {
    return 'B';
  } else if (r == 0xff && g == 0xff && b == 0xff && a == 0xff) {
    return 'W';
  } else {
    return '?';
  }
}

static char *buffer_to_text(UtObject *buffer) {
  size_t width = ut_image_buffer_get_width(buffer);
  size_t height = ut_image_buffer_get_height(buffer);
  UtObjectRef string = ut_string_new("");
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      uint8_t r, g, b, a;
      ut_rgba8888_buffer_get_pixel(buffer, x, y, &r, &g, &b, &a);
      ut_string_append_code_point(string, get_pixel_text(r, g, b, a));
    }
  }
  return ut_string_take_text(string);
}

static void test_mesh(size_t width, size_t height, const char *verticies_text,
                      const char *triangles_text, const char *color_text,
                      const char *expected_buffer_text) {
  UtObjectRef buffer = ut_rgba8888_buffer_new(width, height);
  UtObjectRef verticies = parse_verticies(verticies_text);
  UtObjectRef triangles = parse_triangles(triangles_text);
  UtObjectRef mesh = ut_mesh_new(verticies, triangles);
  UtObjectRef color = ut_color_new_from_hex_string(color_text);

  ut_drawable_render_mesh(buffer, mesh, color);
  ut_cstring_ref buffer_text = buffer_to_text(buffer);
  ut_assert_cstring_equal(buffer_text, expected_buffer_text);
}

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

  // FIXME: Anti-clockwise

  // Triangle with all diagonal lines:
  test_mesh(5, 5, "0,0,5,1,2,4", "0,1,2", "#ffffff",
            "WWW  "
            " WWWW"
            " WWW "
            "  W  "
            "     ");

  // Covering pixel:
  // +---+ +---+ +---+ +---+
  // |#\ | |###| | /#| | ^ |
  // |##>| |\#/| |<##| |/#\|
  // |#/ | | V | | \#| |###|
  // +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0,1,0.5,0,1", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "0,0,1,0,0.5,1", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "1,0,1,1,0,0.5", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "0.5,0,1,1,0,1", "0,1,2", "#ffffff", "W");
  test_mesh(5, 5, "0,0,5,2.2,0,5", "0,1,2", "#ffffff",
            "W    "
            "WWW  "
            "WWWW "
            "WWW  "
            "W    ");
  test_mesh(5, 5, "0,0,5,0,2.5,5", "0,1,2", "#ffffff",
            "WWWWW"
            " WWW "
            " WWW "
            "  W  "
            "  W  ");
  test_mesh(5, 5, "5,0,5,5,0,2.5", "0,1,2", "#ffffff",
            "    W"
            "  WWW"
            "WWWWW"
            "  WWW"
            "    W");
  test_mesh(5, 5, "2.5,0,5,5,0,5", "0,1,2", "#ffffff",
            "  W  "
            "  W  "
            " WWW "
            " WWW "
            "WWWWW");

  // Triangle outside dimensions of area.
  test_mesh(1, 1, "-1,-1,2,-1,-1,2", "0,1,2", "#ffffff", "W");

  // Missing pixel:
  // +---+ +---+ +---+ +---+
  // |/  | |  \| |   | |   |
  // |   | |   | |   | |   |
  // |   | |   | |  /| |\  |
  // +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0,0.5,0,0,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0.5,0,1,0,1,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,0.5,1,1,0.5,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0.5,1,0,1,0,0.5", "0,1,2", "#ffffff", " ");

  // Diagonal crossing through pixels:
  // +---+ +---+ +---+ +---+
  // |##/| |\##| |  /| |\  |
  // |#/ | | \#| | /#| |#\ |
  // |/  | |  \| |/##| |##\|
  // +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0,1,0,0,1", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "0,0,1,0,1,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,0,1,1,0,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,0,1,1,0,1", "0,1,2", "#ffffff", "W");
  test_mesh(5, 5, "0,0,5,0,0,5", "0,1,2", "#ffffff",
            "WWWWW"
            "WWWW "
            "WWW  "
            "WW   "
            "W    ");
  test_mesh(5, 5, "0,0,5,0,5,5", "0,1,2", "#ffffff",
            " WWWW"
            "  WWW"
            "   WW"
            "    W"
            "     ");
  test_mesh(5, 5, "5,0,5,5,0,5", "0,1,2", "#ffffff",
            "     "
            "    W"
            "   WW"
            "  WWW"
            " WWWW");
  test_mesh(5, 5, "0,0,5,5,0,5", "0,1,2", "#ffffff",
            "W    "
            "WW   "
            "WWW  "
            "WWWW "
            "WWWWW");

  // Orthogal crossing through pixel:
  // +---+ +---+ +---+ +---+
  // |/| | |/#\| | |\| |   |
  // |#| | |---| | |#| |---|
  // |\| | |   | | |/| |\#/|
  // +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0.5,0.5,0,0.5,1", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "0.5,0,1,0.5,0,0.5", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "1,0.5,0.5,1,0.5,0", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0.5,1,0,0.5,1,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(5, 5, "0,2.5,2.5,0,2.5,5", "0,1,2", "#ffffff",
            "  W  "
            " WW  "
            "WWW  "
            " WW  "
            "  W  ");
  test_mesh(5, 5, "2.5,0,5,2.5,0,2.5", "0,1,2", "#ffffff",
            "  W  "
            " WWW "
            "WWWWW"
            "     "
            "     ");
  test_mesh(5, 5, "5,2.5,2.5,5,2.5,0", "0,1,2", "#ffffff",
            "     "
            "   W "
            "   WW"
            "   W "
            "     ");
  test_mesh(5, 5, "2.5,5,0,2.5,5,2.5", "0,1,2", "#ffffff",
            "     "
            "     "
            "     "
            " WWW "
            "  W  ");

  // Point meeting at pixel:
  // +---+ +---+ +---+ +---+
  // |\  | |\#/| |  /| |   |
  // |#> | | V | | <#| | ^ |
  // |/  | |   | |  \| |/#\|
  // +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,1,0,0,0.5,0.5", "0,1,2", "#ffffff", "W");
  test_mesh(1, 1, "0,0,1,0,0.5,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,0,1,1,0.5,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,1,0,1,0.5,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(5, 5, "0,5,0,0,2.5,2.5", "0,1,2", "#ffffff",
            "W    "
            "WW   "
            "WWW  "
            "WW   "
            "W    ");
  test_mesh(5, 5, "0,0,5,0,2.5,2.5", "0,1,2", "#ffffff",
            " WWWW"
            "  WW "
            "     "
            "     "
            "     ");
  test_mesh(5, 5, "5,0,5,5,2.5,2.5", "0,1,2", "#ffffff",
            "     "
            "    W"
            "   WW"
            "    W"
            "     ");
  test_mesh(5, 5, "5,5,0,5,2.5,2.5", "0,1,2", "#ffffff",
            "     "
            "     "
            "     "
            "  WW "
            " WWWW");

  // Points:
  // +---+ +---+ +---+ +---+ +---+
  // |.  | |  .| |   | |   | |   |
  // |   | |   | |   | |   | | . |
  // |   | |   | |  .| |.  | |   |
  // +---+ +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0,0,0,0,0", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,0,1,0,1,0", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,1,1,1,1,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,1,0,1,0,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0.5,0.5,0.5,0.5,0.5,0.5", "0,1,2", "#ffffff", " ");

  // Lines:
  // +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+
  // ||  | |---| |  || |   | | | | |   | |\  | |  /|
  // ||  | |   | |  || |   | | | | |---| | \ | | / |
  // ||  | |   | |  || |---| | | | |   | |  \| |/  |
  // +---+ +---+ +---+ +---+ +---+ +---+ +---+ +---+
  test_mesh(1, 1, "0,0,0,1,0,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,0,1,0,1,0", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,0,1,1,1,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "1,1,1,0,1,0", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0.5,0,0.5,1,0.5,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,0.5,1,0.5,1,0.5", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,0,1,1,1,1", "0,1,2", "#ffffff", " ");
  test_mesh(1, 1, "0,1,1,0,1,0", "0,1,2", "#ffffff", " ");

  return 0;
}
