#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t width;
  uint32_t height;
  UtObject *commands;
} UtTvgImage;

static void ut_tvg_image_cleanup(UtObject *object) {
  UtTvgImage *self = (UtTvgImage *)object;
  ut_object_unref(self->commands);
}

static char *ut_tvg_image_to_string(UtObject *object) {
  UtTvgImage *self = (UtTvgImage *)object;
  ut_cstring_ref commands_string = ut_object_to_string(self->commands);
  return ut_cstring_new_printf(
      "<UtTvgImage>(width: %d, height: %d, commands: %s)", self->width,
      self->height, commands_string);
}

static UtObjectInterface object_interface = {.type_name = "UtTvgImage",
                                             .cleanup = ut_tvg_image_cleanup,
                                             .to_string =
                                                 ut_tvg_image_to_string};

UtObject *ut_tvg_image_new(uint32_t width, uint32_t height,
                           UtObject *commands) {
  UtObject *object = ut_object_new(sizeof(UtTvgImage), &object_interface);
  UtTvgImage *self = (UtTvgImage *)object;

  assert(width > 0);
  assert(height > 0);

  self->width = width;
  self->height = height;
  self->commands = ut_object_ref(commands);

  return object;
}

uint32_t ut_tvg_image_get_width(UtObject *object) {
  assert(ut_object_is_tvg_image(object));
  UtTvgImage *self = (UtTvgImage *)object;
  return self->width;
}

uint32_t ut_tvg_image_get_height(UtObject *object) {
  assert(ut_object_is_tvg_image(object));
  UtTvgImage *self = (UtTvgImage *)object;
  return self->height;
}

UtObject *ut_tvg_image_get_commands(UtObject *object) {
  assert(ut_object_is_tvg_image(object));
  UtTvgImage *self = (UtTvgImage *)object;
  return self->commands;
}

bool ut_object_is_tvg_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
