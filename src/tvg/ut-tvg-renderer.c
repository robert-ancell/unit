#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Image being rendered.
  UtObject *image;
} UtTvgRenderer;

static void ut_tvg_renderer_cleanup(UtObject *object) {
  UtTvgRenderer *self = (UtTvgRenderer *)object;
  ut_object_unref(self->image);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTvgRenderer", .cleanup = ut_tvg_renderer_cleanup};

UtObject *ut_tvg_renderer_new(UtObject *image) {
  assert(ut_object_is_tvg_image(image));
  UtObject *object = ut_object_new(sizeof(UtTvgRenderer), &object_interface);
  UtTvgRenderer *self = (UtTvgRenderer *)object;
  self->image = ut_object_ref(image);
  return object;
}

UtObject *ut_tvg_renderer_render(UtObject *object) {
  assert(ut_object_is_tvg_renderer(object));
  UtTvgRenderer *self = (UtTvgRenderer *)object;

  size_t width = ut_tvg_image_get_width(self->image);
  size_t height = ut_tvg_image_get_height(self->image);

  UtObjectRef data = ut_uint8_array_new();
  ut_list_resize(data, width * height * 3);
  uint8_t *buffer = ut_uint8_list_get_writable_data(data);

  size_t offset = 0;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      uint8_t *pixel = buffer + offset;
      pixel[0] = 0;
      pixel[1] = 0;
      pixel[2] = 0;
      offset += 3;
    }
  }

  return ut_object_ref(data);
}

bool ut_object_is_tvg_renderer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
