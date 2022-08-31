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

static int get_hex_digit(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  } else {
    return -1;
  }
}

static int get_hex_value(const char *string) {
  int nibble0 = get_hex_digit(string[0]);
  int nibble1 = get_hex_digit(string[1]);
  if (nibble0 < 0 || nibble1 < 0) {
    return -1;
  }
  return nibble0 << 4 | nibble1;
}

static char *ut_color_to_string(UtObject *object) {
  UtColor *self = (UtColor *)object;
  return ut_cstring_new_printf(
      "<UtColor>(red: %f, green: %f, blue: %f, alpha: %f)", self->red,
      self->green, self->blue, self->alpha);
}

static UtObjectInterface object_interface = {.type_name = "UtColor",
                                             .to_string = ut_color_to_string,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_color_new_rgba(double red, double green, double blue,
                            double alpha) {
  UtObject *object = ut_object_new(sizeof(UtColor), &object_interface);
  UtColor *self = (UtColor *)object;

  self->red = red;
  self->green = green;
  self->blue = blue;
  self->alpha = alpha;

  return object;
}

UtObject *ut_color_new_from_hex_string(const char *hex_string) {
  int red = 255;
  int green = 255;
  int blue = 255;
  int alpha = 255;

  if (!ut_cstring_starts_with(hex_string, "#") ||
      ut_cstring_get_length(hex_string) != 7 ||
      (red = get_hex_value(hex_string + 1)) == -1 ||
      (green = get_hex_value(hex_string + 3)) == -1 ||
      (blue = get_hex_value(hex_string + 5)) == -1) {
    return ut_error_new("Invalid color string");
  }

  return ut_color_new_rgba(red / 255.0, green / 255.0, blue / 255.0,
                           alpha / 255.0);
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
