#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int16_t x_offset;
  int16_t y_offset;
  uint16_t index;
} UtTrueTypeCompositeGlyphComponent;

static char *
ut_true_type_composite_glyph_component_to_string(UtObject *object) {
  UtTrueTypeCompositeGlyphComponent *self =
      (UtTrueTypeCompositeGlyphComponent *)object;
  return ut_cstring_new_printf(
      "<UtTrueTypeCompositeGlyphComponent>(%d, %d, %d)", self->x_offset,
      self->y_offset, self->index);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTrueTypeCompositeGlyphComponent",
    .to_string = ut_true_type_composite_glyph_component_to_string};

UtObject *ut_true_type_composite_glyph_component_new(int16_t x_offset,
                                                     int16_t y_offset,
                                                     uint16_t index) {
  UtObject *object = ut_object_new(sizeof(UtTrueTypeCompositeGlyphComponent),
                                   &object_interface);
  UtTrueTypeCompositeGlyphComponent *self =
      (UtTrueTypeCompositeGlyphComponent *)object;
  self->x_offset = x_offset;
  self->y_offset = y_offset;
  self->index = index;
  return object;
}

int16_t ut_true_type_composite_glyph_component_get_x_offset(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph_component(object));
  UtTrueTypeCompositeGlyphComponent *self =
      (UtTrueTypeCompositeGlyphComponent *)object;
  return self->x_offset;
}

int16_t ut_true_type_composite_glyph_component_get_y_offset(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph_component(object));
  UtTrueTypeCompositeGlyphComponent *self =
      (UtTrueTypeCompositeGlyphComponent *)object;
  return self->y_offset;
}

uint16_t ut_true_type_composite_glyph_component_get_index(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph_component(object));
  UtTrueTypeCompositeGlyphComponent *self =
      (UtTrueTypeCompositeGlyphComponent *)object;
  return self->index;
}

bool ut_object_is_true_type_composite_glyph_component(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
