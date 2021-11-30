#include <stdio.h>

#include "ut.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s font.ttf\n", argv[0]);
    return 1;
  }
  const char *path = argv[1];

  UtObjectRef file = ut_memory_mapped_file_new(path);
  ut_file_open_read(file);
  UtObjectRef decoder = ut_open_type_decoder_new(file);
  ut_open_type_decoder_decode(decoder);
  ut_assert_null_object(ut_open_type_decoder_get_error(decoder));

  UtObject *font = ut_open_type_decoder_get_font(decoder);
  printf("units-per-em: %u\n", ut_open_type_font_get_units_per_em(font));
  UtObject *glyphs = ut_open_type_font_get_glyphs(font);
  size_t glyphs_length = ut_list_get_length(glyphs);
  printf("glyphs:\n");
  for (size_t i = 0; i < glyphs_length; i++) {
    UtObject *glyph = ut_object_list_get_element(glyphs, i);

    if (glyph != NULL) {
      ut_cstring_ref glyph_string = ut_object_to_string(glyph);
      printf("  %zi: %s\n", i, glyph_string);
    }
  }

  UtObjectRef renderer = ut_true_type_glyph_renderer_new(font, 0, 10);
  ut_true_type_glyph_renderer_render(renderer);

  return 0;
}
