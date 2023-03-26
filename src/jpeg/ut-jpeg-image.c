#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint16_t width;
  uint16_t height;
  UtJpegDensityUnits density_units;
  uint16_t horizontal_pixel_density;
  uint16_t vertical_pixel_density;
  size_t n_components;
  UtObject *data;
} UtJpegImage;

static void ut_jpeg_image_cleanup(UtObject *object) {
  UtJpegImage *self = (UtJpegImage *)object;
  ut_object_unref(self->data);
}

static char *ut_jpeg_image_to_string(UtObject *object) {
  UtJpegImage *self = (UtJpegImage *)object;
  return ut_cstring_new_printf("<UtJpegImage>(width: %d, height: %d)",
                               self->width, self->height);
}

static UtObjectInterface object_interface = {.type_name = "UtJpegImage",
                                             .cleanup = ut_jpeg_image_cleanup,
                                             .to_string =
                                                 ut_jpeg_image_to_string,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_jpeg_image_new(uint16_t width, uint16_t height,
                            UtJpegDensityUnits density_units,
                            uint16_t horizontal_pixel_density,
                            uint16_t vertical_pixel_density,
                            size_t n_components, UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtJpegImage), &object_interface);
  UtJpegImage *self = (UtJpegImage *)object;

  assert(ut_list_get_length(data) == width * height * n_components);

  self->width = width;
  self->height = height;
  self->density_units = density_units;
  self->horizontal_pixel_density = horizontal_pixel_density;
  self->vertical_pixel_density = vertical_pixel_density;
  self->height = height;
  self->n_components = n_components;
  self->data = ut_object_ref(data);
  return object;
}

uint16_t ut_jpeg_image_get_width(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->width;
}

uint16_t ut_jpeg_image_get_height(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->height;
}

UtJpegDensityUnits ut_jpeg_image_get_density_units(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->density_units;
}

uint16_t ut_jpeg_image_get_horizontal_pixel_density(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->horizontal_pixel_density;
}

uint16_t ut_jpeg_image_get_vertical_pixel_density(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->vertical_pixel_density;
}

size_t ut_jpeg_image_get_n_components(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->n_components;
}

UtObject *ut_jpeg_image_get_data(UtObject *object) {
  assert(ut_object_is_jpeg_image(object));
  UtJpegImage *self = (UtJpegImage *)object;
  return self->data;
}

bool ut_object_is_jpeg_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
