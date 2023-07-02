#include <assert.h>
#include <stdint.h>

#include "ut.h"

#define BLOCK_TYPE_EXTENSION 0x21
#define BLOCK_TYPE_IMAGE 0x2c
#define BLOCK_TYPE_TRAILER 0x3b

typedef struct {
  UtObject object;
  uint16_t left;
  uint16_t top;
  uint16_t width;
  uint16_t height;
  UtGifDisposalMethod disposal_method;
  uint16_t delay_time;
  UtObject *color_table;
  UtObject *data;
} UtGifImage;

static void ut_gif_image_cleanup(UtObject *object) {
  UtGifImage *self = (UtGifImage *)object;
  ut_object_unref(self->color_table);
  ut_object_unref(self->data);
}

static char *ut_gif_image_to_string(UtObject *object) {
  UtGifImage *self = (UtGifImage *)object;
  return ut_cstring_new_printf("<UtGifImage>(width: %d, height: %d)",
                               self->width, self->height);
}

static UtObjectInterface object_interface = {.type_name = "UtGifImage",
                                             .cleanup = ut_gif_image_cleanup,
                                             .to_string =
                                                 ut_gif_image_to_string};

UtObject *ut_gif_image_new(uint16_t left, uint16_t top, uint16_t width,
                           uint16_t height, UtObject *color_table,
                           UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtGifImage), &object_interface);
  UtGifImage *self = (UtGifImage *)object;

  assert(width > 0);
  assert(height > 0);
  assert(color_table == NULL || ut_list_get_length(color_table) % 3 == 0);

  self->left = left;
  self->top = top;
  self->width = width;
  self->height = height;
  self->color_table = ut_object_ref(color_table);
  self->data = ut_object_ref(data);

  return object;
}

uint16_t ut_gif_image_get_left(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->left;
}

uint16_t ut_gif_image_get_top(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->top;
}

uint16_t ut_gif_image_get_width(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->width;
}

uint16_t ut_gif_image_get_height(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->height;
}

void ut_gif_image_set_disposal_method(UtObject *object,
                                      UtGifDisposalMethod disposal_method) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  self->disposal_method = disposal_method;
}

UtGifDisposalMethod ut_gif_image_get_disposal_method(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->disposal_method;
}

void ut_gif_image_set_delay_time(UtObject *object, uint16_t delay_time) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  self->delay_time = delay_time;
}

uint16_t ut_gif_image_get_delay_time(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->delay_time;
}

UtObject *ut_gif_image_get_color_table(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->color_table;
}

UtObject *ut_gif_image_get_data(UtObject *object) {
  assert(ut_object_is_gif_image(object));
  UtGifImage *self = (UtGifImage *)object;
  return self->data;
}

bool ut_object_is_gif_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
