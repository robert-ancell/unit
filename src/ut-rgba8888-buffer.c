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

static uint8_t quantize_channel(float value) {
  float v = value * 255.0;
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

// FIXME: Set entire row to be more efficient
static void set_pixel(UtRgba8888Buffer *self, uint8_t *data, size_t x, size_t y,
                      uint8_t red8, uint8_t green8, uint8_t blue8,
                      uint8_t alpha8) {
  size_t i = get_offset(self, x, y);
  data[i + 0] = red8;
  data[i + 1] = green8;
  data[i + 2] = blue8;
  data[i + 3] = alpha8;
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

  uint8_t *data = ut_uint8_list_get_writable_data(self->data);

  uint8_t red8 = quantize_channel(ut_color_get_red(color));
  uint8_t green8 = quantize_channel(ut_color_get_green(color));
  uint8_t blue8 = quantize_channel(ut_color_get_blue(color));
  uint8_t alpha8 = quantize_channel(ut_color_get_alpha(color));

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

static void get_vertex(UtObject *verticies, uint16_t index, float *x,
                       float *y) {
  *x = ut_float32_list_get_element(verticies, index * 2);
  *y = ut_float32_list_get_element(verticies, (index * 2) + 1);
}

static void get_triangle(UtObject *verticies, UtObject *triangles, size_t index,
                         float *x0, float *y0, float *x1, float *y1, float *x2,
                         float *y2) {
  uint16_t v0 = ut_uint16_list_get_element(triangles, index * 3);
  uint16_t v1 = ut_uint16_list_get_element(triangles, (index * 3) + 1);
  uint16_t v2 = ut_uint16_list_get_element(triangles, (index * 3) + 2);

  get_vertex(verticies, v0, x0, y0);
  get_vertex(verticies, v1, x1, y1);
  get_vertex(verticies, v2, x2, y2);
}

static void swap_coords(float *x0, float *y0, float *x1, float *y1) {
  float tx = *x0;
  *x0 = *x1;
  *x1 = tx;
  float ty = *y0;
  *y0 = *y1;
  *y1 = ty;
}

static void render_row(UtRgba8888Buffer *self, uint8_t *data, float x0,
                       float x1, size_t y, uint8_t red8, uint8_t green8,
                       uint8_t blue8, uint8_t alpha8) {
  size_t x0_i = (size_t)(x0 + 0.5);
  size_t x1_i = (size_t)(x1 + 0.5);
  for (size_t x = x0_i; x < x1_i; x++) {
    set_pixel(self, data, x, y, red8, green8, blue8, alpha8);
  }
}

static void render_triangle(UtRgba8888Buffer *self, uint8_t *data,
                            UtObject *verticies, UtObject *triangles,
                            size_t index, uint8_t red8, uint8_t green8,
                            uint8_t blue8, uint8_t alpha8) {
  float x0, y0, x1, y1, x2, y2;
  get_triangle(verticies, triangles, index, &x0, &y0, &x1, &y1, &x2, &y2);

  // Order top to bottom.
  if (y1 < y0) {
    swap_coords(&x1, &y1, &x0, &y0);
  }
  if (y2 < y0) {
    swap_coords(&x2, &y2, &x0, &y0);
  }
  if (y2 < y1) {
    swap_coords(&x2, &y2, &x1, &y1);
  }

  // FIXME: Clip

  size_t y0_i = (size_t)(y0 + 0.5);
  size_t y1_i = (size_t)(y1 + 0.5);
  float m_left, m_right;

  if (y0 != y1 && y0 != y2) {
    if (x1 < x2) {
      m_left = (x1 - x0) / (y1 - y0);
      m_right = (x2 - x0) / (y2 - y0);
    } else {
      m_left = (x2 - x0) / (y2 - y0);
      m_right = (x1 - x0) / (y1 - y0);
    }
    for (size_t y = y0_i; y < y1_i; y++) {
      float dy = (y + 0.5) - y0;
      float x_left = x0 + (m_left * dy);
      float x_right = x0 + (m_right * dy);
      render_row(self, data, x_left, x_right, y, red8, green8, blue8, alpha8);
    }
  }

  if (y2 != y1 && y2 != y0) {
    if (x1 < x2) {
      m_left = (x1 - x2) / (y2 - y1);
      m_right = (x0 - x2) / (y2 - y0);
    } else {
      m_left = (x0 - x2) / (y2 - y0);
      m_right = (x1 - x2) / (y2 - y1);
    }
    size_t y2_i = (size_t)(y2 + 0.5);
    for (size_t y = y1_i; y < y2_i; y++) {
      float dy = y2 - (y + 0.5);
      float x_left = x2 + (m_left * dy);
      float x_right = x2 + (m_right * dy);
      render_row(self, data, x_left, x_right, y, red8, green8, blue8, alpha8);
    }
  }
}

static void ut_rgba8888_buffer_render_mesh(UtObject *object, UtObject *mesh,
                                           UtObject *color) {
  UtRgba8888Buffer *self = (UtRgba8888Buffer *)object;

  uint8_t *data = ut_uint8_list_get_writable_data(self->data);

  uint8_t red8 = quantize_channel(ut_color_get_red(color));
  uint8_t green8 = quantize_channel(ut_color_get_green(color));
  uint8_t blue8 = quantize_channel(ut_color_get_blue(color));
  uint8_t alpha8 = quantize_channel(ut_color_get_alpha(color));

  UtObject *verticies = ut_mesh_get_verticies(mesh);
  UtObject *triangles = ut_mesh_get_triangles(mesh);
  size_t n_triangles = ut_list_get_length(triangles) / 3;
  for (size_t i = 0; i < n_triangles; i++) {
    render_triangle(self, data, verticies, triangles, i, red8, green8, blue8,
                    alpha8);
  }
}

static UtImageBufferInterface image_buffer_interface = {
    .get_width = ut_rgba8888_buffer_get_width,
    .get_height = ut_rgba8888_buffer_get_height,
    .get_data = ut_rgba8888_buffer_get_data};

static UtDrawableInterface drawable_interface = {
    .clear = ut_rgba8888_buffer_clear,
    .render_mesh = ut_rgba8888_buffer_render_mesh};

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
