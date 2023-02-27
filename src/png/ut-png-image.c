#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t width;
  uint32_t height;
  uint8_t bit_depth;
  UtPngColourType colour_type;
  UtObject *palette;
  UtObject *data;
} UtPngImage;

static const char *colour_type_to_string(UtPngColourType colour_type) {
  switch (colour_type) {
  case UT_PNG_COLOUR_TYPE_GREYSCALE:
    return "greyscale";
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR:
    return "truecolour";
  case UT_PNG_COLOUR_TYPE_INDEXED_COLOUR:
    return "indexed-colour";
  case UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA:
    return "greyscale-with-alpha";
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA:
    return "truecolour-with-alpha";
  default:
    return "?";
  }
}

static size_t get_n_channels(UtPngColourType colour_type) {
  switch (colour_type) {
  case UT_PNG_COLOUR_TYPE_GREYSCALE:
  case UT_PNG_COLOUR_TYPE_INDEXED_COLOUR:
    return 1;
  case UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA:
    return 2;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR:
    return 3;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA:
    return 4;
  default:
    return 0;
  }
}

static size_t get_row_stride(uint32_t width, UtPngColourType colour_type,
                             uint8_t bit_depth) {
  size_t n_channels = get_n_channels(colour_type);
  return (((size_t)width * bit_depth * n_channels) + 7) / 8;
}

static void ut_png_image_cleanup(UtObject *object) {
  UtPngImage *self = (UtPngImage *)object;
  ut_object_unref(self->palette);
  ut_object_unref(self->data);
}

static char *ut_png_image_to_string(UtObject *object) {
  UtPngImage *self = (UtPngImage *)object;
  return ut_cstring_new_printf(
      "<UtPngImage>(width: %d, height: %d, bit_depth: %d, colour_type: %s)",
      self->width, self->height, self->bit_depth,
      colour_type_to_string(self->colour_type));
}

static UtObjectInterface object_interface = {.type_name = "UtPngImage",
                                             .cleanup = ut_png_image_cleanup,
                                             .to_string =
                                                 ut_png_image_to_string,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_png_image_new(uint32_t width, uint32_t height, uint8_t bit_depth,
                           UtPngColourType colour_type, UtObject *palette,
                           UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtPngImage), &object_interface);
  UtPngImage *self = (UtPngImage *)object;

  assert(width > 0);
  assert(height > 0);
  switch (colour_type) {
  case UT_PNG_COLOUR_TYPE_GREYSCALE:
    assert(bit_depth == 1 || bit_depth == 2 || bit_depth == 4 ||
           bit_depth == 8 || bit_depth == 16);
    // Palette not allowed.
    assert(palette == NULL);
    break;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR:
    assert(bit_depth == 8 || bit_depth == 16);
    break;
  case UT_PNG_COLOUR_TYPE_INDEXED_COLOUR:
    assert(bit_depth == 1 || bit_depth == 2 || bit_depth == 4 ||
           bit_depth == 8);
    // Palette required.
    assert(palette != NULL);
    break;
  case UT_PNG_COLOUR_TYPE_GREYSCALE_WITH_ALPHA:
    assert(bit_depth == 8 || bit_depth == 16);
    // Palette not allowed.
    assert(palette == NULL);
    break;
  case UT_PNG_COLOUR_TYPE_TRUECOLOUR_WITH_ALPHA:
    assert(bit_depth == 8 || bit_depth == 16);
    break;
  default:
    assert(false);
  }
  if (palette != NULL) {
    size_t palette_length = ut_list_get_length(palette);
    assert(palette_length % 3 == 0);
    size_t n_palette_entries = palette_length / 3;
    assert(n_palette_entries >= 1 && n_palette_entries <= 256);
  }
  ut_assert_int_equal(ut_list_get_length(data),
                      height * get_row_stride(width, colour_type, bit_depth));

  self->width = width;
  self->height = height;
  self->bit_depth = bit_depth;
  self->colour_type = colour_type;
  self->palette = ut_object_ref(palette);
  self->data = ut_object_ref(data);

  return object;
}

uint32_t ut_png_image_get_width(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->width;
}

uint32_t ut_png_image_get_height(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->height;
}

uint8_t ut_png_image_get_bit_depth(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->bit_depth;
}

UtPngColourType ut_png_image_get_colour_type(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->colour_type;
}

size_t ut_png_image_get_n_channels(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return get_n_channels(self->colour_type);
}

size_t ut_png_image_get_row_stride(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return get_row_stride(self->width, self->colour_type, self->bit_depth);
}

UtObject *ut_png_image_get_palette(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->palette;
}

UtObject *ut_png_image_get_data(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->data;
}

bool ut_object_is_png_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
