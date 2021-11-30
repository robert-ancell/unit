#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Font being rendered.
  UtObject *font;

  size_t glyph;

  size_t width;
  size_t height;
} UtTrueTypeGlyphRenderer;

static void render_simple_glyph(UtTrueTypeGlyphRenderer *self,
                                UtObject *glyph) {
  int16_t x_min = ut_true_type_simple_glyph_get_x_min(glyph);
  int16_t y_min = ut_true_type_simple_glyph_get_y_min(glyph);
  int16_t x_max = ut_true_type_simple_glyph_get_x_max(glyph);
  int16_t y_max = ut_true_type_simple_glyph_get_y_max(glyph);
  // FIXME
}

static void render_composite_glyph(UtTrueTypeGlyphRenderer *self,
                                   UtObject *glyph) {}

static char *ut_true_type_glyph_renderer_to_string(UtObject *object) {
  UtTrueTypeGlyphRenderer *self = (UtTrueTypeGlyphRenderer *)object;
  ut_cstring_ref font_string = ut_object_to_string(self->font);
  return ut_cstring_new_printf("<UtTrueTypeGlyphRenderer>(%s, %zi)",
                               font_string, self->glyph);
}

static void ut_true_type_glyph_renderer_cleanup(UtObject *object) {
  UtTrueTypeGlyphRenderer *self = (UtTrueTypeGlyphRenderer *)object;
  ut_object_unref(self->font);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTrueTypeGlyphRenderer",
    .to_string = ut_true_type_glyph_renderer_to_string,
    .cleanup = ut_true_type_glyph_renderer_cleanup};

UtObject *ut_true_type_glyph_renderer_new(UtObject *font, size_t glyph,
                                          size_t height_in_pixels) {
  UtObject *object =
      ut_object_new(sizeof(UtTrueTypeGlyphRenderer), &object_interface);
  UtTrueTypeGlyphRenderer *self = (UtTrueTypeGlyphRenderer *)object;
  self->font = ut_object_ref(font);
  self->glyph = glyph;
  self->height = height_in_pixels;
  return object;
}

void ut_true_type_glyph_renderer_render(UtObject *object) {
  assert(ut_object_is_true_type_glyph_renderer(object));
  UtTrueTypeGlyphRenderer *self = (UtTrueTypeGlyphRenderer *)object;

  // FIXME: Check valid number
  UtObject *glyph = ut_object_list_get_element(
      ut_open_type_font_get_glyphs(self->font), self->glyph);
  if (ut_object_is_true_type_simple_glyph(glyph)) {
    render_simple_glyph(self, glyph);
  } else if (ut_object_is_true_type_composite_glyph(glyph)) {
    render_composite_glyph(self, glyph);
  }
}

bool ut_object_is_true_type_glyph_renderer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
