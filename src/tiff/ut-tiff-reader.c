#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "ut.h"

// https://www.itu.int/itudoc/itu-t/com16/tiff-fx/docs/tiff6.pdf

typedef struct {
  UtObject object;

  // Data being read.
  UtObject *data;

  // True if data is in little endian format.
  bool little_endian;

  // Tags in the file.
  UtObject *tags;

  // Offset where first image file directory is.
  uint32_t ifd_offset;

  // Error that occurred during reading.
  UtObject *error;
} UtTiffReader;

static void set_error(UtTiffReader *self, const char *description) {
  if (self->error == NULL) {
    self->error = ut_tiff_error_new(description);
  }
}

static uint8_t read_uint8(UtTiffReader *self, size_t offset) {
  return ut_uint8_list_get_element(self->data, offset);
}

static uint16_t read_uint16(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_uint16_le(self->data, offset);
  } else {
    return ut_uint8_list_get_uint16_be(self->data, offset);
  }
}

static uint32_t read_uint32(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_uint32_le(self->data, offset);
  } else {
    return ut_uint8_list_get_uint32_be(self->data, offset);
  }
}

static float read_float32(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_float32_le(self->data, offset);
  } else {
    return ut_uint8_list_get_float32_be(self->data, offset);
  }
}

static int16_t read_int16(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_int16_le(self->data, offset);
  } else {
    return ut_uint8_list_get_int16_be(self->data, offset);
  }
}

static int32_t read_int32(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_int32_le(self->data, offset);
  } else {
    return ut_uint8_list_get_int32_be(self->data, offset);
  }
}

static double read_float64(UtTiffReader *self, size_t offset) {
  if (self->little_endian) {
    return ut_uint8_list_get_float64_le(self->data, offset);
  } else {
    return ut_uint8_list_get_float64_be(self->data, offset);
  }
}

static bool decode_header(UtTiffReader *self) {
  if (self->ifd_offset != 0) {
    return true;
  }

  size_t data_length = ut_list_get_length(self->data);

  if (data_length < 16) {
    set_error(self, "Insufficient data for header");
    return false;
  }

  uint8_t byte_order0 = read_uint8(self, 0);
  uint8_t byte_order1 = read_uint8(self, 1);

  if (byte_order0 == 'I' && byte_order1 == 'I') {
    self->little_endian = true;
  } else if (byte_order0 == 'M' && byte_order1 == 'M') {
    self->little_endian = false;
  } else {
    set_error(self, "Invalid TIFF byte order");
    return false;
  }

  uint16_t magic_number = read_uint16(self, 2);
  if (magic_number != 42) {
    set_error(self, "Invalid TIFF header");
    return false;
  }

  uint32_t ifd_offset = read_uint32(self, 4);
  if (ifd_offset == 0) {
    set_error(self, "Invalid TIFF IFD offset");
    return false;
  }

  self->ifd_offset = ifd_offset;
  return true;
}

static UtObject *decode_byte_data(UtTiffReader *self, size_t count,
                                  size_t offset) {
  size_t tag_data_offset;
  if (count <= 4) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + count > data_length) {
      set_error(self, "Insufficient data for TIFF byte tag");
    }
  }

  return ut_list_get_sublist(self->data, tag_data_offset, count);
}

static UtObject *decode_ascii_data(UtTiffReader *self, size_t count,
                                   size_t offset) {
  size_t tag_data_offset;
  if (count == 0) {
    set_error(self, "Invalid zero length TIFF ASCII tag");
  } else if (count <= 4) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + count > data_length) {
      set_error(self, "Insufficient data for TIFF ASCII tag");
    }
  }

  if (read_uint8(self, tag_data_offset + count - 1) != '\0') {
    set_error(self, "TIFF ASCII tag not nul terminated");
  }
  for (size_t i = 0; i < count - 1; i++) {
    if (read_uint8(self, tag_data_offset + i) > 127) {
      set_error(self, "TIFF ASCII tag contains invalid data");
    }
  }

  return ut_list_get_sublist(self->data, tag_data_offset, count);
}

static UtObject *decode_short_data(UtTiffReader *self, size_t count,
                                   size_t offset) {
  size_t tag_data_offset;
  if (count <= 2) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + (count * 2) > data_length) {
      set_error(self, "Insufficient data for TIFF short tag");
    }
  }

  UtObject *tag_data = ut_uint16_list_new();
  size_t tag_data_end = tag_data_offset + count * 2;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 2) {
    ut_uint16_list_append(tag_data, read_uint16(self, o));
  }

  return tag_data;
}

static UtObject *decode_long_data(UtTiffReader *self, size_t count,
                                  size_t offset) {
  size_t tag_data_offset;
  if (count <= 1) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + (count * 4) > data_length) {
      set_error(self, "Insufficient data for TIFF long tag");
    }
  }

  UtObject *tag_data = ut_uint32_list_new();
  size_t tag_data_end = tag_data_offset + count * 4;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 4) {
    ut_uint32_list_append(tag_data, read_uint32(self, o));
  }

  return tag_data;
}

static UtObject *decode_rational_data(UtTiffReader *self, size_t count,
                                      size_t offset) {
  size_t tag_data_offset = read_uint32(self, offset);
  size_t data_length = ut_list_get_length(self->data);
  if (tag_data_offset + (count * 8) > data_length) {
    set_error(self, "Insufficient data for TIFF rational tag");
  }

  UtObject *tag_data = ut_float64_list_new();
  size_t tag_data_end = tag_data_offset + count * 8;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 8) {
    double numerator = read_uint32(self, o);
    double denominator = read_uint32(self, o);
    ut_float64_list_append(tag_data, numerator / denominator);
  }

  return tag_data;
}

static UtObject *decode_sbyte_data(UtTiffReader *self, size_t count,
                                   size_t offset) {
  size_t tag_data_offset;
  if (count <= 4) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + count > data_length) {
      set_error(self, "Insufficient data for TIFF sbyte tag");
    }
  }

  // FIXME Use int8 list
  return ut_list_get_sublist(self->data, tag_data_offset, count);
}

static UtObject *decode_undefined_data(UtTiffReader *self, size_t count,
                                       size_t offset) {
  size_t tag_data_offset;
  if (count <= 4) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + count > data_length) {
      set_error(self, "Insufficient data for TIFF undefined tag");
    }
  }

  return ut_list_get_sublist(self->data, tag_data_offset, count);
}

static UtObject *decode_sshort_data(UtTiffReader *self, size_t count,
                                    size_t offset) {
  size_t tag_data_offset;
  if (count <= 2) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + (count * 2) > data_length) {
      set_error(self, "Insufficient data for TIFF sshort tag");
    }
  }

  UtObject *tag_data = ut_int16_list_new();
  size_t tag_data_end = tag_data_offset + count * 2;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 2) {
    ut_int16_list_append(tag_data, read_int16(self, o));
  }

  return tag_data;
}

static UtObject *decode_slong_data(UtTiffReader *self, size_t count,
                                   size_t offset) {
  size_t tag_data_offset;
  if (count <= 1) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + (count * 4) > data_length) {
      set_error(self, "Insufficient data for TIFF slong tag");
    }
  }

  UtObject *tag_data = ut_int32_list_new();
  size_t tag_data_end = tag_data_offset + count * 4;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 4) {
    ut_int32_list_append(tag_data, read_int32(self, o));
  }

  return tag_data;
}

static UtObject *decode_srational_data(UtTiffReader *self, size_t count,
                                       size_t offset) {
  size_t tag_data_offset = read_uint32(self, offset);
  size_t data_length = ut_list_get_length(self->data);
  if (tag_data_offset + (count * 8) > data_length) {
    set_error(self, "Insufficient data for TIFF srational tag");
  }

  UtObject *tag_data = ut_float64_list_new();
  size_t tag_data_end = tag_data_offset + count * 8;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 8) {
    double numerator = read_int32(self, o);
    double denominator = read_int32(self, o);
    ut_float64_list_append(tag_data, numerator / denominator);
  }

  return tag_data;
}

static UtObject *decode_float_data(UtTiffReader *self, size_t count,
                                   size_t offset) {
  size_t tag_data_offset;
  if (count <= 1) {
    tag_data_offset = offset;
  } else {
    tag_data_offset = read_uint32(self, offset);
    size_t data_length = ut_list_get_length(self->data);
    if (tag_data_offset + (count * 4) > data_length) {
      set_error(self, "Insufficient data for TIFF float tag");
    }
  }

  UtObject *tag_data = ut_float32_list_new();
  size_t tag_data_end = tag_data_offset + count * 4;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 4) {
    ut_float32_list_append(tag_data, read_float32(self, o));
  }

  return tag_data;
}

static UtObject *decode_double_data(UtTiffReader *self, size_t count,
                                    size_t offset) {
  size_t tag_data_offset = read_uint32(self, offset);
  size_t data_length = ut_list_get_length(self->data);
  if (tag_data_offset + (count * 8) > data_length) {
    set_error(self, "Insufficient data for TIFF double tag");
  }

  UtObject *tag_data = ut_float64_list_new();
  size_t tag_data_end = tag_data_offset + count * 8;
  for (size_t o = tag_data_offset; o < tag_data_end; o += 4) {
    ut_float64_list_append(tag_data, read_float64(self, o));
  }

  return tag_data;
}

static UtObject *decode_tags(UtTiffReader *self) {
  if (self->tags != NULL) {
    return self->tags;
  }

  if (!decode_header(self)) {
    return NULL;
  }

  size_t data_length = ut_list_get_length(self->data);
  uint32_t ifd_offset = self->ifd_offset;
  UtObjectRef tags = ut_list_new();
  do {
    if (data_length < ifd_offset + 6) {
      set_error(self, "Insufficient data for IFD");
    }

    uint16_t n_entries = read_uint16(self, ifd_offset);
    size_t ifd_length = 2 + n_entries * 12 + 4;
    if (data_length < ifd_offset + ifd_length) {
      set_error(self, "Insufficient data for IFD entries");
    }

    for (size_t i = 0; i < n_entries; i++) {
      size_t entry_offset = ifd_offset + 2 + i * 12;
      uint16_t id = read_uint16(self, entry_offset);
      uint16_t type = read_uint16(self, entry_offset + 2);
      uint32_t count = read_uint32(self, entry_offset + 4);
      size_t value_offset = entry_offset + 8;

      UtObjectRef tag_data = NULL;
      switch (type) {
      case UT_TIFF_TAG_TYPE_BYTE:
        tag_data = decode_byte_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_ASCII:
        tag_data = decode_ascii_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_SHORT:
        tag_data = decode_short_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_LONG:
        tag_data = decode_long_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_RATIONAL:
        tag_data = decode_rational_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_SBYTE:
        tag_data = decode_sbyte_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_UNDEFINED:
        tag_data = decode_undefined_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_SSHORT:
        tag_data = decode_sshort_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_SLONG:
        tag_data = decode_slong_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_SRATIONAL:
        tag_data = decode_srational_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_FLOAT:
        tag_data = decode_float_data(self, count, value_offset);
        break;
      case UT_TIFF_TAG_TYPE_DOUBLE:
        tag_data = decode_double_data(self, count, value_offset);
        break;
      default:
        tag_data = decode_long_data(self, 1, value_offset);
        break;
      }
      if (ut_object_implements_error(tag_data)) {
        return ut_object_ref(tag_data);
      }
      UtObjectRef tag = ut_tiff_tag_new(id, type, tag_data);
      ut_list_append(tags, tag);
    }

    ifd_offset = read_uint32(self, ifd_offset + 2 + n_entries * 12);
  } while (ifd_offset != 0);

  self->tags = ut_object_ref(tags);
  return self->tags;
}

static UtObject *get_tag(UtObject *object, uint16_t id) {
  assert(ut_object_is_tiff_reader(object));
  return ut_tiff_reader_lookup_tag(object, id);
}

static uint16_t get_short(UtObject *object, uint16_t id) {
  UtObject *tag = get_tag(object, id);
  if (tag == NULL) {
    return 0;
  }

  return ut_tiff_tag_get_short(tag, 0);
}

static uint32_t get_short_or_long(UtObject *object, uint16_t id) {
  UtObject *tag = get_tag(object, id);
  if (tag == NULL) {
    return 0;
  }

  return ut_tiff_tag_get_short_or_long(tag, 0);
}

static char *get_ascii(UtObject *object, uint16_t id) {
  UtObject *tag = get_tag(object, id);
  if (tag == NULL) {
    return NULL;
  }

  return ut_tiff_tag_get_ascii(tag);
}

static UtObject *get_concatenated_ascii(UtObject *object, uint16_t id) {
  UtObject *tag = get_tag(object, id);
  if (tag == NULL) {
    return NULL;
  }

  return ut_tiff_tag_get_concatenated_ascii(tag);
}

static void ut_tiff_reader_cleanup(UtObject *object) {
  UtTiffReader *self = (UtTiffReader *)object;
  ut_object_unref(self->data);
  ut_object_unref(self->tags);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtTiffReader",
                                             .cleanup = ut_tiff_reader_cleanup};

UtObject *ut_tiff_reader_new(UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtTiffReader), &object_interface);
  UtTiffReader *self = (UtTiffReader *)object;
  self->data = ut_object_ref(data);
  return object;
}

UtObject *ut_tiff_reader_get_tags(UtObject *object) {
  assert(ut_object_is_tiff_reader(object));
  UtTiffReader *self = (UtTiffReader *)object;
  return decode_tags(self);
}

UtObject *ut_tiff_reader_lookup_tag(UtObject *object, uint16_t id) {
  assert(ut_object_is_tiff_reader(object));
  UtTiffReader *self = (UtTiffReader *)object;
  UtObject *tags = decode_tags(self);
  if (tags == NULL) {
    return NULL;
  }

  size_t tags_length = ut_list_get_length(tags);
  for (size_t i = 0; i < tags_length; i++) {
    UtObjectRef tag = ut_list_get_element(tags, i);
    if (ut_tiff_tag_get_id(tag) == id) {
      return tag;
    }
  }

  return NULL;
}

uint32_t ut_tiff_reader_get_image_width(UtObject *object) {
  return get_short_or_long(object, UT_TIFF_TAG_IMAGE_WIDTH);
}

uint32_t ut_tiff_reader_get_image_length(UtObject *object) {
  return get_short_or_long(object, UT_TIFF_TAG_IMAGE_LENGTH);
}

uint16_t ut_tiff_reader_get_bits_per_sample(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_BITS_PER_SAMPLE);
}

uint16_t ut_tiff_reader_get_compression(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_COMPRESSION);
}

uint16_t ut_tiff_reader_get_photometric_interpretation(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_PHOTOMETRIC_INTERPRETATION);
}

uint16_t ut_tiff_reader_get_thresholding(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_THRESHOLDING);
}

uint16_t ut_tiff_reader_get_cell_width(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_CELL_WIDTH);
}

uint16_t ut_tiff_reader_get_cell_length(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_CELL_LENGTH);
}

uint16_t ut_tiff_reader_get_fill_order(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_FILL_ORDER);
}

char *ut_tiff_reader_get_document_name(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_DOCUMENT_NAME);
}

char *ut_tiff_reader_get_image_description(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_IMAGE_DESCRIPTION);
}

char *ut_tiff_reader_get_make(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_MAKE);
}

char *ut_tiff_reader_get_model(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_MODEL);
}

uint16_t ut_tiff_reader_get_orientation(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_ORIENTATION);
}

uint16_t ut_tiff_reader_get_samples_per_pixel(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_SAMPLES_PER_PIXEL);
}

uint32_t ut_tiff_reader_get_rows_per_strip(UtObject *object) {
  return get_short_or_long(object, UT_TIFF_TAG_ROWS_PER_STRIP);
}

uint16_t ut_tiff_reader_get_planar_configuration(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_PLANAR_CONFIGURATION);
}

char *ut_tiff_reader_get_page_name(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_PAGE_NAME);
}

uint16_t ut_tiff_reader_get_resolution_unit(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_RESOLUTION_UNIT);
}

uint16_t ut_tiff_reader_get_page_number(UtObject *object,
                                        uint16_t *total_pages) {
  // FIXME
  return 0;
}

char *ut_tiff_reader_get_software(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_SOFTWARE);
}

char *ut_tiff_reader_get_date_time(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_DATE_TIME);
}

char *ut_tiff_reader_get_artist(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_ARTIST);
}

char *ut_tiff_reader_get_host_computer(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_HOST_COMPUTER);
}

uint32_t ut_tiff_reader_get_tile_width(UtObject *object) {
  return get_short_or_long(object, UT_TIFF_TAG_TILE_WIDTH);
}

uint32_t ut_tiff_reader_get_tile_length(UtObject *object) {
  return get_short_or_long(object, UT_TIFF_TAG_TILE_LENGTH);
}

uint16_t ut_tiff_reader_get_ink_set(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_INK_SET);
}

UtObject *ut_tiff_reader_get_ink_names(UtObject *object) {
  return get_concatenated_ascii(object, UT_TIFF_TAG_INK_NAMES);
}

uint16_t ut_tiff_reader_get_number_of_inks(UtObject *object) {
  return get_short(object, UT_TIFF_TAG_NUMBER_OF_INKS);
}

char *ut_tiff_reader_get_target_printer(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_TARGET_PRINTER);
}

char *ut_tiff_reader_get_copyright(UtObject *object) {
  return get_ascii(object, UT_TIFF_TAG_COPYRIGHT);
}

UtObject *ut_tiff_reader_get_subfile(UtObject *object, uint32_t offset) {
  assert(ut_object_is_tiff_reader(object));
  UtTiffReader *self = (UtTiffReader *)object;

  if (!decode_header(self)) {
    return NULL;
  }

  UtObject *subfile_object =
      ut_object_new(sizeof(UtTiffReader), &object_interface);
  UtTiffReader *subfile_self = (UtTiffReader *)object;
  subfile_self->data = ut_object_ref(self->data);
  subfile_self->little_endian = self->little_endian;
  subfile_self->ifd_offset = offset;
  return subfile_object;
}

UtObject *ut_tiff_reader_get_error(UtObject *object) {
  assert(ut_object_is_tiff_reader(object));
  UtTiffReader *self = (UtTiffReader *)object;
  decode_tags(self);
  return self->error;
}

bool ut_object_is_tiff_reader(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
