#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  size_t width;
  size_t height;
  UtObject *data;
} UtRgba8888Buffer;

static double clamp(double value, double max) {
  if (value < 0) {
    return 0;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
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

static size_t get_offset(UtRgba8888Buffer *self, size_t x, size_t y) {
  return (y * self->width * 4) + (x * 4);
}

static void set_pixel(UtRgba8888Buffer *self, uint8_t *data, size_t x, size_t y,
                      uint8_t red8, uint8_t green8, uint8_t blue8,
                      uint8_t alpha8) {
  size_t i = get_offset(self, x, y);
  data[i + 0] = red8;
  data[i + 1] = green8;
  data[i + 2] = blue8;
  data[i + 3] = alpha8;
}

static void render_box(UtRgba8888Buffer *self, size_t left, size_t top,
                       size_t right, size_t bottom, UtObject *color) {
  uint8_t red8 = quantize_channel(ut_color_get_red(color));
  uint8_t green8 = quantize_channel(ut_color_get_green(color));
  uint8_t blue8 = quantize_channel(ut_color_get_blue(color));
  uint8_t alpha8 = quantize_channel(ut_color_get_alpha(color));

  uint8_t *data = ut_uint8_list_get_writable_data(self->data);
  size_t row_step = (self->width - (right - left)) * 4;
  size_t i = (top * self->width * 4) + (left * 4);
  for (size_t y = top; y < bottom; y++) {
    for (size_t x = left; x < right; x++) {
      data[i + 0] = red8;
      data[i + 1] = green8;
      data[i + 2] = blue8;
      data[i + 3] = alpha8;
      i += 4;
    }
    i += row_step;
  }
}

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

static void ut_rgba8888_buffer_clear(UtObject *object, UtObject *color) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  render_box(self, 0, 0, self->width, self->height, color);
}

static void ut_rgba8888_buffer_render_box(UtObject *object, double x, double y,
                                          double width, double height,
                                          UtObject *color) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;

  double left = clamp(x, self->width);
  double top = clamp(y, self->height);
  double right = clamp(x + width, self->width);
  double bottom = clamp(y + height, self->height);

  render_box(self, round(left), round(top), round(right), round(bottom), color);
}

static UtImageBufferInterface image_buffer_interface = {
    .get_width = ut_rgba8888_buffer_get_width,
    .get_height = ut_rgba8888_buffer_get_height,
    .get_data = ut_rgba8888_buffer_get_data};

static UtDrawableInterface drawable_interface = {
    .clear = ut_rgba8888_buffer_clear, ut_rgba8888_buffer_render_box};

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

void ut_rgba8888_buffer_set_pixel(UtObject *object, size_t x, size_t y,
                                  uint8_t red, uint8_t green, uint8_t blue,
                                  uint8_t alpha) {
  ut_assert_true(ut_object_is_rgba8888_buffer(object));
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  uint8_t *data = ut_uint8_list_get_writable_data(self->data);
  set_pixel(self, data, x, y, red, green, blue, alpha);
}

void ut_rgba8888_buffer_get_pixel(UtObject *object, size_t x, size_t y,
                                  uint8_t *red, uint8_t *green, uint8_t *blue,
                                  uint8_t *alpha) {
  ut_assert_true(ut_object_is_rgba8888_buffer(object));
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;
  uint8_t *data = ut_uint8_list_get_writable_data(self->data);
  if (x >= self->width || y >= self->height) {
    *red = *green = *blue = *alpha = 0;
    return;
  }
  size_t i = get_offset(self, x, y);
  *red = data[i + 0];
  *green = data[i + 1];
  *blue = data[i + 2];
  *alpha = data[i + 3];
}

bool ut_object_is_rgba8888_buffer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
