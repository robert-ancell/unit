#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  double red;
  double green;
  double blue;
  double alpha;
} UtColor;

static char *ut_color_to_string(UtObject *object) {
  UtColor *self = (UtColor *)object;
  return ut_cstring_new_printf(
      "<UtColor>(red: %f, green: %f, blue: %f, alpha: %f)", self->red,
      self->green, self->blue, self->alpha);
}

static UtObjectInterface object_interface = {.type_name = "UtColor",
                                             .to_string = ut_color_to_string,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_color_new(double red, double green, double blue, double alpha) {
  UtObject *object = ut_object_new(sizeof(UtColor), &object_interface);
  UtColor *self = (UtColor *)object;

  self->red = red;
  self->green = green;
  self->blue = blue;
  self->alpha = alpha;

  return object;
}

double ut_color_get_red(UtObject *object) {
  assert(ut_object_is_color(object));
  UtColor *self = (UtColor *)object;
  return self->red;
}

double ut_color_get_green(UtObject *object) {
  assert(ut_object_is_color(object));
  UtColor *self = (UtColor *)object;
  return self->green;
}

double ut_color_get_blue(UtObject *object) {
  assert(ut_object_is_color(object));
  UtColor *self = (UtColor *)object;
  return self->blue;
}

double ut_color_get_alpha(UtObject *object) {
  assert(ut_object_is_color(object));
  UtColor *self = (UtColor *)object;
  return self->alpha;
}

bool ut_object_is_color(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
