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
  UtObject *contours;
} UtTrueTypeSimpleGlyph;

static char *ut_true_type_simple_glyph_to_string(UtObject *object) {
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  ut_cstring_ref contours_string = ut_object_to_string(self->contours);
  return ut_cstring_new_printf(
      "<UtTrueTypeSimpleGlyph>(%d, %d, %d, %d, %u, %d, %s)", self->x_min,
      self->y_min, self->x_max, self->y_max, self->advance_width,
      self->left_side_bearing, contours_string);
}

static void ut_true_type_simple_glyph_cleanup(UtObject *object) {
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  ut_object_unref(self->contours);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTrueTypeSimpleGlyph",
    .to_string = ut_true_type_simple_glyph_to_string,
    .cleanup = ut_true_type_simple_glyph_cleanup};

UtObject *ut_true_type_simple_glyph_new(int16_t x_min, int16_t y_min,
                                        int16_t x_max, int16_t y_max,
                                        uint16_t advance_width,
                                        int16_t left_side_bearing,
                                        UtObject *contours) {
  UtObject *object =
      ut_object_new(sizeof(UtTrueTypeSimpleGlyph), &object_interface);
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  self->x_min = x_min;
  self->y_min = y_min;
  self->x_max = x_max;
  self->y_max = y_max;
  self->advance_width = advance_width;
  self->left_side_bearing = left_side_bearing;
  self->contours = ut_object_ref(contours);
  return object;
}

int16_t ut_true_type_simple_glyph_get_x_min(UtObject *object) {
  assert(ut_object_is_true_type_simple_glyph(object));
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  return self->x_min;
}

int16_t ut_true_type_simple_glyph_get_y_min(UtObject *object) {
  assert(ut_object_is_true_type_simple_glyph(object));
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  return self->y_min;
}

int16_t ut_true_type_simple_glyph_get_x_max(UtObject *object) {
  assert(ut_object_is_true_type_simple_glyph(object));
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  return self->x_max;
}

int16_t ut_true_type_simple_glyph_get_y_max(UtObject *object) {
  assert(ut_object_is_true_type_simple_glyph(object));
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  return self->y_max;
}

UtObject *ut_true_type_simple_glyph_get_contours(UtObject *object) {
  assert(ut_object_is_true_type_simple_glyph(object));
  UtTrueTypeSimpleGlyph *self = (UtTrueTypeSimpleGlyph *)object;
  return self->contours;
}

bool ut_object_is_true_type_simple_glyph(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
