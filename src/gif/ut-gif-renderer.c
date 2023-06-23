#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  // Decoded image being rendered.
  UtObject *decoder;
} UtGifRenderer;

static void ut_gif_renderer_cleanup(UtObject *object) {
  UtGifRenderer *self = (UtGifRenderer *)object;
  ut_object_unref(self->decoder);
}

static UtObjectInterface object_interface = {.type_name = "UtGifRenderer",
                                             .cleanup = ut_gif_renderer_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_gif_renderer_new(UtObject *decoder) {
  assert(ut_object_is_gif_decoder(decoder));
  UtObject *object = ut_object_new(sizeof(UtGifRenderer), &object_interface);
  UtGifRenderer *self = (UtGifRenderer *)object;
  self->decoder = ut_object_ref(decoder);
  return object;
}

UtObject *ut_gif_renderer_render(UtObject *object) {
  assert(ut_object_is_gif_renderer(object));
  UtGifRenderer *self = (UtGifRenderer *)object;

  size_t width = ut_gif_decoder_get_width(self->decoder);
  size_t height = ut_gif_decoder_get_height(self->decoder);

  UtObjectRef data = ut_uint8_array_new();
  ut_list_resize(data, width * height * 3);
  uint8_t *buffer = ut_uint8_list_get_writable_data(data);

  size_t offset = 0;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      uint8_t *pixel = buffer + offset;
      // FIXME: Use correct color
      pixel[0] = 0;
      pixel[1] = 0;
      pixel[2] = 0;
      offset += 3;
    }
  }

  UtObject *images = ut_gif_decoder_get_images(self->decoder);
  size_t images_length = ut_list_get_length(images);
  UtObject *last_image = NULL;
  for (size_t i = 0; i < images_length; i++) {
    if (last_image != NULL) {
      size_t image_top, image_left, image_width, image_height;
      switch (ut_gif_image_get_disposal_method(last_image)) {
      case UT_GIF_DISPOSAL_METHOD_NONE:
      case UT_GIF_DISPOSAL_METHOD_DO_NOT_DISPOSE:
        // No action required.
        break;
      case UT_GIF_DISPOSAL_METHOD_RESTORE_TO_BACKGROUND:
        image_top = ut_gif_image_get_top(last_image);
        image_left = ut_gif_image_get_left(last_image);
        image_width = ut_gif_image_get_width(last_image);
        image_height = ut_gif_image_get_height(last_image);
        // FIXME: crop
        for (size_t y = 0; y < image_height; y++) {
          size_t iy = image_top + y;
          for (size_t x = 0; x < image_width; x++) {
            size_t ix = image_left + x;
            size_t pixel_offset = (iy * width) + ix;
            uint8_t *pixel = buffer + pixel_offset * 3;
            // FIXME: Use correct color
            pixel[0] = 0;
            pixel[1] = 0;
            pixel[2] = 0;
          }
        }
        break;
      case UT_GIF_DISPOSAL_METHOD_RESTORE_TO_PREVIOUS:
        break;
      }
    }

    UtObject *image = ut_object_list_get_element(images, i);
    size_t image_top = ut_gif_image_get_top(image);
    size_t image_left = ut_gif_image_get_left(image);
    size_t image_width = ut_gif_image_get_width(image);
    size_t image_height = ut_gif_image_get_height(image);
    const uint8_t *image_data =
        ut_uint8_list_get_data(ut_gif_image_get_data(image));
    const uint8_t *color_table =
        ut_uint8_list_get_data(ut_gif_image_get_color_table(image));

    // Only use image if inside final dimensions.
    if (image_left >= width || image_top >= image_height) {
      continue;
    }
    size_t image_cropped_width =
        image_left + image_width > width ? width - image_left : image_width;
    size_t image_cropped_height =
        image_top + image_height > height ? height - image_top : image_height;

    for (size_t y = 0; y < image_cropped_height; y++) {
      size_t iy = image_top + y;
      for (size_t x = 0; x < image_cropped_width; x++) {
        size_t ix = image_left + x;
        uint8_t color_index = image_data[(y * image_width) + x];
        const uint8_t *color = &color_table[color_index * 3];
        size_t pixel_offset = (iy * width) + ix;
        uint8_t *pixel = buffer + pixel_offset * 3;
        pixel[0] = color[0];
        pixel[1] = color[1];
        pixel[2] = color[2];
      }
    }

    last_image = image;
  }

  return ut_object_ref(data);
}

bool ut_object_is_gif_renderer(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
