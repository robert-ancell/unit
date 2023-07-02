#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t width;
  uint32_t height;
  uint8_t bit_depth;
  UtPngColorType color_type;
  UtObject *palette;
  UtObject *background_color;
  UtObject *text;
  UtObject *international_keywords;
  UtObject *international_text;
  UtObject *data;
} UtPngImage;

static const char *color_type_to_string(UtPngColorType color_type) {
  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
    return "greyscale";
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    return "truecolor";
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    return "indexed-color";
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    return "greyscale-with-alpha";
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    return "truecolor-with-alpha";
  default:
    return "?";
  }
}

static size_t get_n_channels(UtPngColorType color_type) {
  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    return 1;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    return 2;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    return 3;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    return 4;
  default:
    return 0;
  }
}

static size_t get_row_stride(uint32_t width, uint8_t bit_depth,
                             size_t n_channels) {
  return (((size_t)width * bit_depth * n_channels) + 7) / 8;
}

static void add_rgba(uint8_t *rgba_data, size_t *rgba_offset, uint8_t r,
                     uint8_t g, uint8_t b, uint8_t a) {
  rgba_data[(*rgba_offset)++] = r;
  rgba_data[(*rgba_offset)++] = g;
  rgba_data[(*rgba_offset)++] = b;
  rgba_data[(*rgba_offset)++] = a;
}

// Calculate value that multiples a sample of [bit_depth] to 8 bits.
// Requires [bit_depth] is 1, 2, 4, or 8;
static uint8_t get_sample_multiplier(size_t bit_depth) {
  return 255 / ((1 << bit_depth) - 1);
}

static void convert_greyscale_to_rgba(size_t width, size_t height,
                                      size_t bit_depth, const uint8_t *data,
                                      uint8_t *rgba_data) {
  // Cases when maximum one sample per byte.
  if (bit_depth == 8) {
    size_t rgba_offset = 0;
    size_t data_length = width * height;
    for (size_t data_offset = 0; data_offset < data_length; data_offset++) {
      uint8_t v = data[data_offset];
      add_rgba(rgba_data, &rgba_offset, v, v, v, 0xff);
    }
    return;
  } else if (bit_depth == 16) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 2;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 2) {
      uint16_t d0 = data[data_offset];
      uint16_t d1 = data[data_offset + 1];
      uint8_t v = (d0 << 8 | d1) / 257;
      add_rgba(rgba_data, &rgba_offset, v, v, v, 0xff);
    }
    return;
  }

  // Cases when multiple samples per byte.
  uint8_t sample_multiplier = get_sample_multiplier(bit_depth);
  size_t row_stride = get_row_stride(width, bit_depth, 1);
  size_t samples_per_byte = 8 / bit_depth;
  uint8_t mask = 0xff >> (8 - bit_depth);
  size_t data_offset = 0;
  size_t rgba_offset = 0;
  for (size_t y = 0; y < height; y++) {
    size_t x = 0;
    for (size_t row_offset = 0; row_offset < row_stride; row_offset++) {
      uint8_t d = data[data_offset++];
      for (size_t i = 0; i < samples_per_byte && x < width; i++) {
        size_t shift = 8 - (bit_depth * (i + 1));
        uint8_t v = ((d >> shift) & mask) * sample_multiplier;
        add_rgba(rgba_data, &rgba_offset, v, v, v, 0xff);
        x++;
      }
    }
  }
}

static void convert_truecolor_to_rgba(size_t width, size_t height,
                                      size_t bit_depth, const uint8_t *data,
                                      uint8_t *rgba_data) {
  if (bit_depth == 8) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 3;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 3) {
      uint8_t r = data[data_offset];
      uint8_t g = data[data_offset + 1];
      uint8_t b = data[data_offset + 2];
      add_rgba(rgba_data, &rgba_offset, r, g, b, 0xff);
    }
  } else if (bit_depth == 16) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 6;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 6) {
      uint16_t d0 = data[data_offset];
      uint16_t d1 = data[data_offset + 1];
      uint16_t d2 = data[data_offset + 2];
      uint16_t d3 = data[data_offset + 3];
      uint16_t d4 = data[data_offset + 4];
      uint16_t d5 = data[data_offset + 5];
      uint8_t r = (d0 << 8 | d1) / 257;
      uint8_t g = (d2 << 8 | d3) / 257;
      uint8_t b = (d4 << 8 | d5) / 257;
      add_rgba(rgba_data, &rgba_offset, r, g, b, 0xff);
    }
  }
}

static void add_indexed_rgba(uint8_t *rgba_data, size_t *rgba_offset,
                             const uint8_t *palette, size_t palette_length,
                             uint8_t index) {
  uint8_t r, g, b;
  if (index < palette_length) {
    r = palette[index * 3];
    g = palette[index * 3 + 1];
    b = palette[index * 3 + 2];
  } else {
    r = g = b = 0;
  }
  add_rgba(rgba_data, rgba_offset, r, g, b, 0xff);
}

static void convert_indexed_to_rgba(size_t width, size_t height,
                                    size_t bit_depth, const uint8_t *data,
                                    const uint8_t *palette,
                                    size_t palette_length, uint8_t *rgba_data) {
  // Cases when maximum one sample per byte.
  if (bit_depth == 8) {
    size_t rgba_offset = 0;
    size_t data_length = width * height;
    for (size_t data_offset = 0; data_offset < data_length; data_offset++) {
      uint8_t index = data[data_offset];
      add_indexed_rgba(rgba_data, &rgba_offset, palette, palette_length, index);
    }
    return;
  }

  // Cases when multiple samples per byte.
  size_t row_stride = get_row_stride(width, bit_depth, 1);
  size_t samples_per_byte = 8 / bit_depth;
  uint8_t mask = 0xff >> (8 - bit_depth);
  size_t data_offset = 0;
  size_t rgba_offset = 0;
  for (size_t y = 0; y < height; y++) {
    size_t x = 0;
    for (size_t row_offset = 0; row_offset < row_stride; row_offset++) {
      uint8_t d = data[data_offset++];
      for (size_t i = 0; i < samples_per_byte && x < width; i++) {
        size_t shift = 8 - (bit_depth * (i + 1));
        uint8_t index = (d >> shift) & mask;
        add_indexed_rgba(rgba_data, &rgba_offset, palette, palette_length,
                         index);
        x++;
      }
    }
  }
}

static void convert_greyscale_with_alpha_to_rgba(size_t width, size_t height,
                                                 size_t bit_depth,
                                                 const uint8_t *data,
                                                 uint8_t *rgba_data) {
  // Cases when maximum one sample per byte.
  if (bit_depth == 8) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 2;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 2) {
      uint8_t v = data[data_offset];
      uint8_t a = data[data_offset + 1];
      add_rgba(rgba_data, &rgba_offset, v, v, v, a);
    }
    return;
  } else if (bit_depth == 16) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 4;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 4) {
      uint16_t d0 = data[data_offset];
      uint16_t d1 = data[data_offset + 1];
      uint16_t d2 = data[data_offset + 2];
      uint16_t d3 = data[data_offset + 3];
      uint8_t v = (d0 << 8 | d1) / 257;
      uint8_t a = (d2 << 8 | d3) / 257;
      add_rgba(rgba_data, &rgba_offset, v, v, v, a);
    }
    return;
  }
}

static void convert_truecolor_with_alpha_to_rgba(size_t width, size_t height,
                                                 size_t bit_depth,
                                                 const uint8_t *data,
                                                 uint8_t *rgba_data) {
  if (bit_depth == 8) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 4;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 4) {
      uint8_t r = data[data_offset];
      uint8_t g = data[data_offset + 1];
      uint8_t b = data[data_offset + 2];
      uint8_t a = data[data_offset + 3];
      add_rgba(rgba_data, &rgba_offset, r, g, b, a);
    }
  } else if (bit_depth == 16) {
    size_t rgba_offset = 0;
    size_t data_length = width * height * 8;
    for (size_t data_offset = 0; data_offset < data_length; data_offset += 8) {
      uint16_t d0 = data[data_offset];
      uint16_t d1 = data[data_offset + 1];
      uint16_t d2 = data[data_offset + 2];
      uint16_t d3 = data[data_offset + 3];
      uint16_t d4 = data[data_offset + 4];
      uint16_t d5 = data[data_offset + 5];
      uint16_t d6 = data[data_offset + 6];
      uint16_t d7 = data[data_offset + 7];
      uint8_t r = (d0 << 8 | d1) / 257;
      uint8_t g = (d2 << 8 | d3) / 257;
      uint8_t b = (d4 << 8 | d5) / 257;
      uint8_t a = (d6 << 8 | d7) / 257;
      add_rgba(rgba_data, &rgba_offset, r, g, b, a);
    }
  }
}

static void ut_png_image_cleanup(UtObject *object) {
  UtPngImage *self = (UtPngImage *)object;
  ut_object_unref(self->palette);
  ut_object_unref(self->background_color);
  ut_object_unref(self->text);
  ut_object_unref(self->international_keywords);
  ut_object_unref(self->international_text);
  ut_object_unref(self->data);
}

static char *ut_png_image_to_string(UtObject *object) {
  UtPngImage *self = (UtPngImage *)object;
  return ut_cstring_new_printf(
      "<UtPngImage>(width: %d, height: %d, bit_depth: %d, color_type: %s)",
      self->width, self->height, self->bit_depth,
      color_type_to_string(self->color_type));
}

static UtObjectInterface object_interface = {.type_name = "UtPngImage",
                                             .cleanup = ut_png_image_cleanup,
                                             .to_string =
                                                 ut_png_image_to_string};

UtObject *ut_png_image_new(uint32_t width, uint32_t height, uint8_t bit_depth,
                           UtPngColorType color_type, UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtPngImage), &object_interface);
  UtPngImage *self = (UtPngImage *)object;

  assert(width > 0);
  assert(height > 0);
  switch (color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
    assert(bit_depth == 1 || bit_depth == 2 || bit_depth == 4 ||
           bit_depth == 8 || bit_depth == 16);
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    assert(bit_depth == 8 || bit_depth == 16);
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    assert(bit_depth == 1 || bit_depth == 2 || bit_depth == 4 ||
           bit_depth == 8);
    break;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    assert(bit_depth == 8 || bit_depth == 16);
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    assert(bit_depth == 8 || bit_depth == 16);
    break;
  default:
    assert(false);
  }
  ut_assert_int_equal(
      ut_list_get_length(data),
      height * get_row_stride(width, bit_depth, get_n_channels(color_type)));

  self->width = width;
  self->height = height;
  self->bit_depth = bit_depth;
  self->color_type = color_type;
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

UtPngColorType ut_png_image_get_color_type(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->color_type;
}

size_t ut_png_image_get_n_channels(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return get_n_channels(self->color_type);
}

size_t ut_png_image_get_row_stride(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return get_row_stride(self->width, self->bit_depth,
                        get_n_channels(self->color_type));
}

void ut_png_image_set_palette(UtObject *object, UtObject *palette) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;

  size_t palette_length = ut_list_get_length(palette);
  assert(palette_length % 3 == 0);
  size_t n_palette_entries = palette_length / 3;
  assert(n_palette_entries >= 1 && n_palette_entries <= 256);

  ut_object_unref(self->palette);
  self->palette = ut_object_ref(palette);
}

UtObject *ut_png_image_get_palette(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->palette;
}

void ut_png_image_set_background_color(UtObject *object,
                                       UtObject *background_color) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;

  size_t background_color_length = ut_list_get_length(background_color);
  switch (self->color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    assert(self->bit_depth == 16 ? background_color_length == 2
                                 : background_color_length == 1);
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    assert(self->bit_depth == 16 ? background_color_length == 6
                                 : background_color_length == 3);
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    assert(background_color_length == 1);
    break;
  default:
    assert(false);
    break;
  }

  ut_object_unref(self->background_color);
  self->background_color = ut_object_ref(background_color);
}

UtObject *ut_png_image_get_background_color(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->background_color;
}

void ut_png_image_set_text(UtObject *object, const char *keyword,
                           const char *text) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  if (self->text == NULL) {
    self->text = ut_map_new();
  }
  ut_map_insert_string_take(self->text, keyword, ut_string_new(text));
}

void ut_png_image_set_international_text(UtObject *object, const char *keyword,
                                         const char *language,
                                         const char *translated_keyword,
                                         const char *text) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;

  ut_cstring_ref lower_language = ut_cstring_new_lowercase(language);

  if (self->international_keywords == NULL) {
    self->international_keywords = ut_map_new();
  }
  UtObjectRef language_keywords =
      ut_map_lookup_string(self->international_keywords, lower_language);
  if (language_keywords == NULL) {
    language_keywords = ut_map_new();
    ut_map_insert_string(self->international_keywords, lower_language,
                         language_keywords);
  }
  ut_map_insert_string_take(language_keywords, keyword,
                            ut_string_new(translated_keyword));

  if (self->international_text == NULL) {
    self->international_text = ut_map_new();
  }
  UtObjectRef language_text =
      ut_map_lookup_string(self->international_text, lower_language);
  if (language_text == NULL) {
    language_text = ut_map_new();
    ut_map_insert_string(self->international_text, lower_language,
                         language_text);
  }
  ut_map_insert_string_take(language_text, keyword, ut_string_new(text));
}

const char *ut_png_image_get_text(UtObject *object, const char *keyword) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  if (self->text == NULL) {
    return NULL;
  }
  UtObjectRef text = ut_map_lookup_string(self->text, keyword);
  return text != NULL ? ut_string_get_text(text) : NULL;
}

const char *
ut_png_image_get_international_text(UtObject *object, const char *keyword,
                                    const char *language,
                                    const char **translated_keyword) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;

  ut_cstring_ref lower_language = ut_cstring_new_lowercase(language);

  if (translated_keyword != NULL) {
    UtObjectRef language_keywords =
        self->international_keywords != NULL
            ? ut_map_lookup_string(self->international_keywords, lower_language)
            : NULL;
    UtObject *translated_keyword_string =
        language_keywords != NULL
            ? ut_map_lookup_string(language_keywords, keyword)
            : NULL;
    *translated_keyword = translated_keyword_string != NULL
                              ? ut_string_get_text(translated_keyword_string)
                              : NULL;
  }

  UtObjectRef language_text =
      self->international_text != NULL
          ? ut_map_lookup_string(self->international_text, lower_language)
          : NULL;
  UtObjectRef text = language_text != NULL
                         ? ut_map_lookup_string(language_text, keyword)
                         : NULL;
  return text != NULL ? ut_string_get_text(text) : NULL;
}

UtObject *ut_png_image_get_data(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;
  return self->data;
}

UtObject *ut_png_image_to_rgba(UtObject *object) {
  assert(ut_object_is_png_image(object));
  UtPngImage *self = (UtPngImage *)object;

  // Already in desired format.
  if (self->color_type == UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA &&
      self->bit_depth == 8) {
    return ut_object_ref(self->data);
  }

  const uint8_t *data = ut_uint8_list_get_data(self->data);

  UtObjectRef rgba = ut_uint8_array_new_sized(self->width * self->height * 4);
  uint8_t *rgba_data = ut_uint8_list_get_writable_data(rgba);

  switch (self->color_type) {
  case UT_PNG_COLOR_TYPE_GREYSCALE:
    convert_greyscale_to_rgba(self->width, self->height, self->bit_depth, data,
                              rgba_data);
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR:
    convert_truecolor_to_rgba(self->width, self->height, self->bit_depth, data,
                              rgba_data);
    break;
  case UT_PNG_COLOR_TYPE_INDEXED_COLOR:
    // FIXME: Extend palette if < 256 to avoid any errors. Or validate indexes.
    convert_indexed_to_rgba(self->width, self->height, self->bit_depth, data,
                            ut_uint8_list_get_data(self->palette),
                            ut_list_get_length(self->palette) / 3, rgba_data);
    break;
  case UT_PNG_COLOR_TYPE_GREYSCALE_WITH_ALPHA:
    convert_greyscale_with_alpha_to_rgba(self->width, self->height,
                                         self->bit_depth, data, rgba_data);
    break;
  case UT_PNG_COLOR_TYPE_TRUECOLOR_WITH_ALPHA:
    convert_truecolor_with_alpha_to_rgba(self->width, self->height,
                                         self->bit_depth, data, rgba_data);
    break;
  }

  return ut_object_ref(rgba);
}

bool ut_object_is_png_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
