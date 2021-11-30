#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  int16_t x_min;
  int16_t y_min;
  int16_t x_max;
  int16_t y_max;
  uint16_t advance_width;
  int16_t left_side_bearing;
  UtObject *components;
} UtTrueTypeCompositeGlyph;

static char *ut_true_type_composite_glyph_to_string(UtObject *object) {
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  ut_cstring_ref components_string = ut_object_to_string(self->components);
  return ut_cstring_new_printf(
      "<UtTrueTypeCompositeGlyph>(%d, %d, %d, %d, %u, %d, %s)", self->x_min,
      self->y_min, self->x_max, self->y_max, self->advance_width,
      self->left_side_bearing, components_string);
}

static void ut_true_type_composite_glyph_cleanup(UtObject *object) {
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  ut_object_unref(self->components);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTrueTypeCompositeGlyph",
    .to_string = ut_true_type_composite_glyph_to_string,
    .cleanup = ut_true_type_composite_glyph_cleanup};

UtObject *ut_true_type_composite_glyph_new(int16_t x_min, int16_t y_min,
                                           int16_t x_max, int16_t y_max,
                                           uint16_t advance_width,
                                           int16_t left_side_bearing,
                                           UtObject *components) {
  UtObject *object =
      ut_object_new(sizeof(UtTrueTypeCompositeGlyph), &object_interface);
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  self->x_min = x_min;
  self->y_min = y_min;
  self->x_max = x_max;
  self->y_max = y_max;
  self->advance_width = advance_width;
  self->left_side_bearing = left_side_bearing;
  self->components = ut_object_ref(components);
  return object;
}

int16_t ut_true_type_composite_glyph_get_x_min(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph(object));
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  return self->x_min;
}

int16_t ut_true_type_composite_glyph_get_y_min(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph(object));
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  return self->y_min;
}

int16_t ut_true_type_composite_glyph_get_x_max(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph(object));
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  return self->x_max;
}

int16_t ut_true_type_composite_glyph_get_y_max(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph(object));
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  return self->y_max;
}

UtObject *ut_true_type_composite_glyph_get_components(UtObject *object) {
  assert(ut_object_is_true_type_composite_glyph(object));
  UtTrueTypeCompositeGlyph *self = (UtTrueTypeCompositeGlyph *)object;
  return self->components;
}

bool ut_object_is_true_type_composite_glyph(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
