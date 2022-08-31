#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  size_t width;
  size_t height;
  UtObject *data;
} UtRgba8888Buffer;

static void ut_rgba8888_buffer_cleanup(UtObject *object) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  ut_object_unref(self->data);
}

static char *ut_rgba8888_buffer_to_string(UtObject *object) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  return ut_cstring_new_printf("<UtRgba8888Buffer>(width: %zi, height: %zi)",
                               self->width, self->height);
}

static size_t ut_rgba8888_buffer_get_width(UtObject *object) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  return self->width;
}

static size_t ut_rgba8888_buffer_get_height(UtObject *object) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  return self->height;
}

static UtObject *ut_rgba8888_buffer_get_data(UtObject *object) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  return self->data;
}

static uint8_t quantize_channel(double value) {
  double v = value * 255.0;
  if (v > 255.0) {
    v = 255.0;
  }
  if (v < 0) {
    v = 0;
  }
  return (int)v;
}

static void ut_rgba8888_buffer_clear(UtObject *object, UtObject *color) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  uint8_t red8 = quantize_channel(ut_color_get_red(color));
  uint8_t green8 = quantize_channel(ut_color_get_green(color));
  uint8_t blue8 = quantize_channel(ut_color_get_blue(color));
  uint8_t alpha8 = quantize_channel(ut_color_get_alpha(color));

  uint8_t *data = ut_uint8_array_get_data(self->data);
  size_t i = 0;
  for (size_t y = 0; y < self->height; y++) {
    for (size_t x = 0; x < self->width; x++) {
      data[i + 0] = red8;
      data[i + 1] = green8;
      data[i + 2] = blue8;
      data[i + 3] = alpha8;
      i += 4;
    }
  }
}

static UtImageBufferInterface image_buffer_interface = {
    .get_width = ut_rgba8888_buffer_get_width,
    .get_height = ut_rgba8888_buffer_get_height,
    .get_data = ut_rgba8888_buffer_get_data};

static UtDrawableInterface drawable_interface = {.clear =
                                                     ut_rgba8888_buffer_clear};

static UtObjectInterface object_interface = {
    .type_name = "UtRgba8888Buffer",
    .cleanup = ut_rgba8888_buffer_cleanup,
    .to_string = ut_rgba8888_buffer_to_string,
    .interfaces = {{&ut_image_buffer_id, &image_buffer_interface},
                   {&ut_drawable_id, &drawable_interface},
                   {NULL, NULL}}};

UtObject *ut_rgba8888_buffer_new(size_t width, size_t height) {
  UtObject *object = ut_object_new(sizeof(UtRgba8888Buffer), &object_interface);
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;

  assert(width > 0);
  assert(height > 0);

  self->width = width;
  self->height = height;
  self->data = ut_uint8_array_new();
  ut_list_resize(self->data, width * height * 4);
  return object;
}

bool ut_object_is_rgba8888_buffer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
