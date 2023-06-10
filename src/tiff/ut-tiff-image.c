#include <assert.h>
#include <stdint.h>

#include "ut.h"

typedef struct {
  UtObject object;
  uint32_t width;
  uint32_t length;
  UtTiffPhotometricInterpretation photometric_interpretation;
  UtTiffPlanarConfiguration planar_configuration;
  uint16_t bits_per_sample;
  uint16_t samples_per_pixel;
  UtObject *color_map;
  UtObject *data;
} UtTiffImage;

static size_t get_row_stride(uint32_t width, size_t bits_per_sample,
                             size_t samples_per_pixel) {
  return (((size_t)width * bits_per_sample * samples_per_pixel) + 7) / 8;
}

static bool get_short_tag(UtObject *reader, uint16_t id, uint16_t *value,
                          bool required, uint16_t default_value) {
  UtObject *tag = ut_tiff_reader_get_tag(reader, id);
  if (tag == NULL) {
    if (required) {
      return false;
    }
    *value = default_value;
    return true;
  }

  if (ut_tiff_tag_get_type(tag) != UT_TIFF_TAG_TYPE_SHORT ||
      ut_tiff_tag_get_count(tag) != 1) {
    return false;
  }
  *value = ut_tiff_tag_get_short(tag, 0);
  return true;
}

static bool get_short_or_long_tag(UtObject *reader, uint16_t id,
                                  uint32_t *value, bool required,
                                  uint32_t default_value) {
  UtObject *tag = ut_tiff_reader_get_tag(reader, id);
  if (tag == NULL) {
    if (required) {
      return false;
    }
    *value = default_value;
    return true;
  }

  if (ut_tiff_tag_get_count(tag) != 1) {
    return false;
  }

  switch (ut_tiff_tag_get_type(tag)) {
  case UT_TIFF_TAG_TYPE_SHORT:
    *value = ut_tiff_tag_get_short(tag, 0);
    return true;
  case UT_TIFF_TAG_TYPE_LONG:
    *value = ut_tiff_tag_get_long(tag, 0);
    return true;
  default:
    return false;
  }
}

static bool decode_pack_bits(UtObject *input, UtObject *output) {
  size_t input_length = ut_list_get_length(input);

  for (size_t i = 0; i < input_length; i++) {
    int8_t b = (int8_t)ut_uint8_list_get_element(input, i);
    size_t count, repeat_count;
    if (b >= 0) {
      count = b + 1;
      repeat_count = 1;
    } else if (b == -128) {
      count = 0;
      repeat_count = 0;
    } else {
      count = 1;
      repeat_count = 1 - b;
    }

    if (i + count >= input_length) {
      return false;
    }
    for (size_t j = 0; j < count; j++) {
      i++;
      uint8_t value = ut_uint8_list_get_element(input, i);
      for (size_t k = 0; k < repeat_count; k++) {
        ut_uint8_list_append(output, value);
      }
    }
  }

  return true;
}

static bool decode_lzw(UtObject *input, UtObject *output) {
  UtObjectRef input_stream = ut_list_input_stream_new(input);
  UtObjectRef lzw_decoder = ut_lzw_decoder_new_msb(input_stream);
  UtObjectRef decoded_data = ut_input_stream_read_sync(lzw_decoder);
  if (ut_object_implements_error(decoded_data)) {
    return false;
  }

  ut_list_append_list(output, decoded_data);

  return true;
}

static bool decode_deflate(UtObject *input, UtObject *output) {
  UtObjectRef input_stream = ut_list_input_stream_new(input);
  UtObjectRef zlib_decoder = ut_zlib_decoder_new(input_stream);
  UtObjectRef decoded_data = ut_input_stream_read_sync(zlib_decoder);
  if (ut_object_implements_error(decoded_data)) {
    return false;
  }

  ut_list_append_list(output, decoded_data);

  return true;
}

static bool decode_horizontal_differencing(
    size_t width, size_t length, UtTiffPlanarConfiguration planar_configuration,
    size_t bits_per_sample, size_t samples_per_pixel, UtObject *image_data) {
  // FIXME: Support other bit depths.
  if (bits_per_sample != 8) {
    return false;
  }

  uint8_t *d = ut_uint8_list_get_writable_data(image_data);
  if (planar_configuration == UT_TIFF_PLANAR_CONFIGURATION_CHUNKY) {
    size_t offset = 0;
    for (size_t y = 0; y < length; y++) {
      offset += samples_per_pixel;
      for (size_t x = 1; x < width; x++) {
        for (size_t s = 0; s < samples_per_pixel; s++) {
          d[offset + s] += d[offset + s - samples_per_pixel];
        }
        offset += samples_per_pixel;
      }
    }
  } else if (planar_configuration == UT_TIFF_PLANAR_CONFIGURATION_PLANAR) {
    size_t offset = 0;
    for (size_t s = 0; s < samples_per_pixel; s++) {
      for (size_t y = 0; y < length; y++) {
        offset++;
        for (size_t x = 1; x < width; x++) {
          d[offset] += d[offset - 1];
          offset++;
        }
      }
    }
  }

  return true;
}

static void ut_tiff_image_cleanup(UtObject *object) {
  UtTiffImage *self = (UtTiffImage *)object;
  ut_object_unref(self->color_map);
  ut_object_unref(self->data);
}

static char *ut_tiff_image_to_string(UtObject *object) {
  UtTiffImage *self = (UtTiffImage *)object;
  return ut_cstring_new_printf("<UtTiffImage>(width: %d, length: %d)",
                               self->width, self->length);
}

static UtObjectInterface object_interface = {.type_name = "UtTiffImage",
                                             .cleanup = ut_tiff_image_cleanup,
                                             .to_string =
                                                 ut_tiff_image_to_string,
                                             .interfaces = {{NULL, NULL}}};

UtObject *
ut_tiff_image_new(uint32_t width, uint32_t length,
                  UtTiffPhotometricInterpretation photometric_interpretation,
                  UtTiffPlanarConfiguration planar_configuration,
                  uint16_t bits_per_sample, uint16_t samples_per_pixel,
                  UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtTiffImage), &object_interface);
  UtTiffImage *self = (UtTiffImage *)object;

  assert(width > 0);
  assert(length > 0);
  // FIXMEut_assert_int_equal(ut_list_get_length(data), length * width * 3);

  self->width = width;
  self->length = length;
  self->photometric_interpretation = photometric_interpretation;
  self->planar_configuration = planar_configuration;
  self->bits_per_sample = bits_per_sample;
  self->samples_per_pixel = samples_per_pixel;
  self->data = ut_object_ref(data);

  return object;
}

UtObject *ut_tiff_image_new_from_data(UtObject *data) {
  UtObjectRef reader = ut_tiff_reader_new(data);
  UtObject *error = ut_tiff_reader_get_error(reader);
  if (error != NULL) {
    return error;
  }

  uint32_t image_length;
  if (!get_short_or_long_tag(reader, UT_TIFF_TAG_IMAGE_LENGTH, &image_length,
                             true, 0)) {
    return ut_tiff_error_new("Invalid TIFF image length tag");
  }
  uint32_t image_width;
  if (!get_short_or_long_tag(reader, UT_TIFF_TAG_IMAGE_WIDTH, &image_width,
                             true, 0)) {
    return ut_tiff_error_new("Invalid TIFF image width tag");
  }

  uint16_t samples_per_pixel;
  if (!get_short_tag(reader, UT_TIFF_TAG_SAMPLES_PER_PIXEL, &samples_per_pixel,
                     false, 1)) {
    return ut_tiff_error_new("Invalid TIFF samples per pixel tag");
  }
  UtObject *bits_per_sample_tag =
      ut_tiff_reader_get_tag(reader, UT_TIFF_TAG_BITS_PER_SAMPLE);
  if (bits_per_sample_tag == NULL ||
      ut_tiff_tag_get_type(bits_per_sample_tag) != UT_TIFF_TAG_TYPE_SHORT ||
      ut_tiff_tag_get_count(bits_per_sample_tag) != samples_per_pixel) {
    return ut_tiff_error_new("Invalid TIFF bits per sample tag");
  }

  uint16_t photometric_interpretation_value;
  if (!get_short_tag(reader, UT_TIFF_TAG_PHOTOMETRIC_INTERPRETATION,
                     &photometric_interpretation_value, true, 0)) {
    return ut_tiff_error_new("Invalid TIFF photometric interpretation tag");
  }
  UtTiffPhotometricInterpretation photometric_interpretation =
      photometric_interpretation_value;

  uint16_t planar_configuration_value;
  if (!get_short_tag(reader, UT_TIFF_TAG_PLANAR_CONFIGURATION,
                     &planar_configuration_value, false, 1)) {
    return ut_tiff_error_new("Invalid TIFF planar configuration tag");
  }
  UtTiffPhotometricInterpretation planar_configuration =
      planar_configuration_value;
  switch (planar_configuration) {
  case UT_TIFF_PLANAR_CONFIGURATION_CHUNKY:
  case UT_TIFF_PLANAR_CONFIGURATION_PLANAR:
    break;
  default:
    return ut_tiff_error_new("Unsupported TIFF planar configuration");
  }

  uint16_t resolution_unit;
  if (!get_short_tag(reader, UT_TIFF_TAG_RESOLUTION_UNIT, &resolution_unit,
                     false, 2)) {
    return ut_tiff_error_new("Invalid TIFF resolution unit tag");
  }

  uint16_t predictor_value;
  if (!get_short_tag(reader, UT_TIFF_TAG_PREDICTOR, &predictor_value, false,
                     1)) {
    return ut_tiff_error_new("Invalid TIFF predictor tag");
  }
  UtTiffPredictor predictor = predictor_value;
  switch (predictor) {
  case UT_TIFF_PREDICTOR_NONE:
  case UT_TIFF_PREDICTOR_HORIZONTAL_DIFFERENCING:
    break;
  default:
    return ut_tiff_error_new("Unsupported TIFF predictor");
  }

  uint16_t compression_value;
  if (!get_short_tag(reader, UT_TIFF_TAG_COMPRESSION, &compression_value, true,
                     0)) {
    return ut_tiff_error_new("Invalid TIFF compression tag");
  }
  UtTiffCompression compression = compression_value;

  uint32_t rows_per_strip;
  if (!get_short_or_long_tag(reader, UT_TIFF_TAG_ROWS_PER_STRIP,
                             &rows_per_strip, true, 0)) {
    return ut_tiff_error_new("Invalid TIFF rows per strip tag");
  }
  UtObject *strip_offsets_tag =
      ut_tiff_reader_get_tag(reader, UT_TIFF_TAG_STRIP_OFFSETS);
  uint16_t strip_offsets_type = ut_tiff_tag_get_type(strip_offsets_tag);
  if (strip_offsets_tag == NULL ||
      !(strip_offsets_type == UT_TIFF_TAG_TYPE_SHORT ||
        strip_offsets_type == UT_TIFF_TAG_TYPE_LONG)) {
    return ut_tiff_error_new("Invalid TIFF strip offsets tag");
  }
  size_t n_strips = ut_tiff_tag_get_count(strip_offsets_tag);
  UtObject *strip_byte_counts_tag =
      ut_tiff_reader_get_tag(reader, UT_TIFF_TAG_STRIP_BYTE_COUNTS);
  uint16_t strip_byte_counts_type = ut_tiff_tag_get_type(strip_byte_counts_tag);
  if (strip_byte_counts_tag == NULL ||
      !(strip_byte_counts_type == UT_TIFF_TAG_TYPE_SHORT ||
        strip_byte_counts_type == UT_TIFF_TAG_TYPE_LONG) ||
      ut_tiff_tag_get_count(strip_byte_counts_tag) != n_strips) {
    return ut_tiff_error_new("Invalid TIFF strip byte counts tag");
  }

  if (image_length == 0 || image_width == 0) {
    return ut_tiff_error_new("Invalid TIFF image dimensions");
  }

  uint16_t bits_per_sample;
  UtObject *color_map = NULL;
  switch (photometric_interpretation) {
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_WHITE_IS_ZERO:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_BLACK_IS_ZERO:
    if (samples_per_pixel < 1) {
      return ut_tiff_error_new(
          "Invalid samples per pixel in bilevel/grayscale TIFF image");
    }
    bits_per_sample = ut_tiff_tag_get_short(bits_per_sample_tag, 0);
    if (bits_per_sample != 1 && bits_per_sample != 4 && bits_per_sample != 8) {
      return ut_tiff_error_new(
          "Unsupported bits per sample in bilevel/grayscale TIFF image");
    }
    if (compression != UT_TIFF_COMPRESSION_UNCOMPRESSED &&
        compression != UT_TIFF_COMPRESSION_PACK_BITS &&
        compression != UT_TIFF_COMPRESSION_LZW &&
        compression != UT_TIFF_COMPRESSION_DEFLATE) {
      return ut_tiff_error_new("Unsupported TIFF compression");
    }
    break;
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB:
    if (samples_per_pixel < 3) {
      return ut_tiff_error_new("Invalid samples per pixel in RGB TIFF image");
    }
    if (ut_tiff_tag_get_short(bits_per_sample_tag, 0) != 8 ||
        ut_tiff_tag_get_short(bits_per_sample_tag, 1) != 8 ||
        ut_tiff_tag_get_short(bits_per_sample_tag, 2) != 8) {
      return ut_tiff_error_new("Unsupported bits per sample in RGB TIFF image");
    }
    bits_per_sample = 8;
    if (compression != UT_TIFF_COMPRESSION_UNCOMPRESSED &&
        compression != UT_TIFF_COMPRESSION_PACK_BITS &&
        compression != UT_TIFF_COMPRESSION_LZW &&
        compression != UT_TIFF_COMPRESSION_DEFLATE) {
      return ut_tiff_error_new("Unsupported TIFF compression");
    }
    break;
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_RGB_PALETTE:
    if (samples_per_pixel != 1) {
      return ut_tiff_error_new(
          "Invalid samples per pixel in color_map color TIFF image");
    }
    bits_per_sample = ut_tiff_tag_get_short(bits_per_sample_tag, 0);
    if (bits_per_sample != 4 && bits_per_sample != 8) {
      return ut_tiff_error_new(
          "Unsupported bits per sample in color_map color TIFF image");
    }
    if (compression != UT_TIFF_COMPRESSION_UNCOMPRESSED &&
        compression != UT_TIFF_COMPRESSION_PACK_BITS) {
      return ut_tiff_error_new("Unsupported TIFF compression");
    }

    UtObject *color_map_tag =
        ut_tiff_reader_get_tag(reader, UT_TIFF_TAG_COLOR_MAP);
    if (color_map_tag == NULL) {
      return ut_tiff_error_new(
          "Missing colormap in color_map color TIFF image");
    }
    if (ut_tiff_tag_get_count(color_map_tag) != 3 * (1 << bits_per_sample)) {
      return ut_tiff_error_new(
          "Invalid length colormap in color_map color TIFF image");
    }
    color_map = ut_tiff_tag_get_data(color_map_tag);
    break;
  default:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_TRANSPARENCY_MASK:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_CMYK:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_YCBCR:
  case UT_TIFF_PHOTOMETRIC_INTERPRETATION_CIELAB:
    return ut_tiff_error_new("Unsupported TIFF photometric interpretation");
  }

  UtObjectRef image_data = ut_uint8_array_new();
  size_t data_length = ut_list_get_length(data);
  for (size_t i = 0; i < n_strips; i++) {
    uint32_t offset = ut_tiff_tag_get_short_or_long(strip_offsets_tag, i);
    uint32_t byte_count =
        ut_tiff_tag_get_short_or_long(strip_byte_counts_tag, i);
    if (offset + byte_count > data_length) {
      return ut_tiff_error_new("Invalid TIFF strip");
    }

    UtObjectRef strip = ut_list_get_sublist(data, offset, byte_count);
    switch (compression) {
    case UT_TIFF_COMPRESSION_UNCOMPRESSED:
      ut_list_append_list(image_data, strip);
      break;
    case UT_TIFF_COMPRESSION_PACK_BITS:
      if (!decode_pack_bits(strip, image_data)) {
        return ut_tiff_error_new("Invalid TIFF PackBits data");
      }
      break;
    case UT_TIFF_COMPRESSION_LZW:
      if (!decode_lzw(strip, image_data)) {
        return ut_tiff_error_new("Invalid TIFF LZW data");
      }
      break;
    case UT_TIFF_COMPRESSION_DEFLATE:
      if (!decode_deflate(strip, image_data)) {
        return ut_tiff_error_new("Invalid TIFF Deflate data");
      }
      break;
    default:
      assert(false);
    }
  }

  if (predictor == UT_TIFF_PREDICTOR_HORIZONTAL_DIFFERENCING) {
    decode_horizontal_differencing(image_width, image_length,
                                   planar_configuration, bits_per_sample,
                                   samples_per_pixel, image_data);
  }

  UtObject *image = ut_tiff_image_new(
      image_width, image_length, photometric_interpretation,
      planar_configuration, bits_per_sample, samples_per_pixel, image_data);
  if (color_map != NULL) {
    ut_tiff_image_set_color_map(image, color_map);
  }
  return image;
}

uint32_t ut_tiff_image_get_width(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->width;
}

uint32_t ut_tiff_image_get_length(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->length;
}

UtTiffPhotometricInterpretation
ut_tiff_image_get_photometric_interpretation(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->photometric_interpretation;
}

UtTiffPlanarConfiguration
ut_tiff_image_get_planar_configuration(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->planar_configuration;
}

uint16_t ut_tiff_image_get_bits_per_sample(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->bits_per_sample;
}

uint16_t ut_tiff_image_get_samples_per_pixel(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->samples_per_pixel;
}

size_t ut_tiff_image_get_row_stride(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return get_row_stride(self->width, self->bits_per_sample,
                        self->samples_per_pixel);
}

void ut_tiff_image_set_color_map(UtObject *object, UtObject *color_map) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;

  size_t color_map_length = ut_list_get_length(color_map);
  assert(color_map_length % 3 == 0);
  size_t n_color_map_entries = color_map_length / 3;
  assert(n_color_map_entries == 1 << self->bits_per_sample);

  ut_object_unref(self->color_map);
  self->color_map = ut_object_ref(color_map);
}

UtObject *ut_tiff_image_get_color_map(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->color_map;
}

UtObject *ut_tiff_image_get_data(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;
  return self->data;
}

UtObject *ut_tiff_image_to_rgba(UtObject *object) {
  assert(ut_object_is_tiff_image(object));
  UtTiffImage *self = (UtTiffImage *)object;

  // Already in desired format. FIXME
  return ut_object_ref(self->data);
}

bool ut_object_is_tiff_image(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
