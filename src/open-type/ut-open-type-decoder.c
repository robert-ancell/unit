#include <assert.h>

#include "ut.h"

// https://developer.apple.com/fonts/TrueType-Reference-Manual/
// https://learn.microsoft.com/en-us/typography/opentype/spec/

#define VERSION_OPEN_TYPE 0x00010000

// Required tables
#define TABLE_CHARACTER_TO_GLYPH_INDEX_MAPPING 0x636d6170 // "cmap"
#define TABLE_GLYPH_DATA 0x676c7966                       // "glyf"
#define TABLE_FONT_HEADER 0x68656164                      // "head"
#define TABLE_HORIZONTAL_HEADER 0x68686561                // "hhea"
#define TABLE_HORIZONTAL_METRICS 0x686d7478               // "hmtx"
#define TABLE_INDEX_TO_LOCATION 0x6c6f6361                // "loca"
#define TABLE_MAXIMUM_PROFILE 0x6d617870                  // "maxp"
#define TABLE_NAMING 0x6e616d65                           // "name"
#define TABLE_POSTSCRIPT 0x706f7374                       // "post"

// Optional tables
#define TABLE_CONTROL_VALUE 0x63767420             // "cvt "
#define TABLE_FONT_PROGRAM 0x6670676d              // "fpgm"
#define TABLE_HORIZONTAL_DEVICE_METRICS 0x68646d78 // "hdmx"
#define TABLE_KERNING 0x6b65726e                   // "kern"
#define TABLE_OS2 0x4f532f32                       // "OS/2"
#define TABLE_CONTROL_VALUE_PROGRAM 0x6670676d     // "prep"

// Supported table versions
#define FONT_HEADER_VERSION_MAJOR 1
#define FONT_HEADER_VERSION_MINOR 0
#define HORIZONTAL_HEADER_VERSION_MAJOR 1
#define HORIZONTAL_HEADER_VERSION_MINOR 0

// Supported glyph data format
#define GLYPH_DATA_FORMAT 0

// Supported metric data format
#define METRIC_DATA_FORMAT 0

typedef enum {
  HEADER_FLAG_BASELINE_Y0 = 1 << 0,
  HEADER_FLAG_LEFT_SIDEBEARING_X0 = 1 << 1,
  HEADER_FLAG_INSTRUCTIONS_DEPEND_POINT_SIZE = 1 << 2,
  HEADER_FLAG_INTEGER_SCALING = 1 << 3,
  HEADER_FLAG_INSTRUCTIONS_ALTER_ADVANCE_WIDTH = 1 << 4,
  HEADER_FLAG_VERTICAL = 1 << 5,
  HEADER_FLAG_REQUIRES_LAYOUT = 1 << 7,
  HEADER_FLAG_HAS_METAMORPHOSIS_EFFECTS = 1 << 8,
  HEADER_FLAG_HAS_RTL_GLYPHS = 1 << 9,
  HEADER_FLAG_HAS_INDIC_REARRANGEMENT = 1 << 10,
  HEADER_FLAG_LOSSLESS = 1 << 11,
  HEADER_FLAG_CONVERTED = 1 << 12,
  HEADER_FLAG_OPTIMIZED_CLEARTYPE = 1 << 13,
  HEADER_FLAG_LAST_RESORT = 1 << 14
} HeaderFlag;

typedef enum {
  PLATFORM_ID_UNICODE = 0,
  PLATFORM_ID_MACINTOSH = 1,
  PLATFORM_ID_ISO = 2,
  PLATFORM_ID_WINDOWS = 3,
  PLATFORM_ID_CUSTOM = 4
} PlatformId;

typedef enum {
  UNICODE_PLATFORM_ENCODING_1_0 = 0,
  UNICODE_PLATFORM_ENCODING_1_1 = 1,
  UNICODE_PLATFORM_ENCODING_ISO_IEC_10646 = 2,
  UNICODE_PLATFORM_ENCODING_2_0_BMP = 3,
  UNICODE_PLATFORM_ENCODING_2_0_FULL = 4,
  UNICODE_PLATFORM_ENCODING_VARIATION = 5,
  UNICODE_PLATFORM_ENCODING_FULL = 6
} UnicodePlatformEncoding;

typedef enum {
  WINDOWS_PLATFORM_ENCODING_SYMBOL = 0,
  WINDOWS_PLATFORM_ENCODING_UNICODE_BMP = 1,
  WINDOWS_PLATFORM_ENCODING_SHIFT_JIS = 2,
  WINDOWS_PLATFORM_ENCODING_PRC = 3,
  WINDOWS_PLATFORM_ENCODING_BIG5 = 4,
  WINDOWS_PLATFORM_ENCODING_WANSUNG = 5,
  WINDOWS_PLATFORM_ENCODING_JOHAB = 6,
  WINDOWS_PLATFORM_ENCODING_UNICODE_FULL = 10
} WindowsPlatformEncoding;

typedef enum {
  NAME_ID_COPYRIGHT = 0,
  NAME_ID_FAMILY_NAME = 1,
  NAME_ID_SUBFAMILY_NAME = 2,
  NAME_ID_UNIQUE_ID = 3,
  NAME_ID_FULL_FONT_NAME = 4,
  NAME_ID_VERSION = 5,
  NAME_ID_POSTSCRIPT_NAME = 6,
  NAME_ID_TRADEMARK = 7,
  NAME_ID_MANUFACTURER_NAME = 8,
  NAME_ID_DESIGNER = 9,
  NAME_ID_DESCRIPTION = 10,
  NAME_ID_VENDOR_URL = 11,
  NAME_ID_DESIGNER_URL = 12,
  NAME_ID_LICENSE_DESCRIPTION = 13,
  NAME_ID_LICENSE_URL = 14,
  NAME_ID_TYPOGRAPHIC_FAMILY_NAME = 16,
  NAME_ID_TYPOGRAPHIC_SUBFAMILY_NAME = 17,
  NAME_ID_COMPATIBLE_FULL_NAME = 18,
  NAME_ID_SAMPLE_TEXT = 19,
  NAME_ID_POSTSCRIPT_CID_FINDFONT_NAME = 20,
  NAME_ID_WWS_FAMILY_NAME = 21,
  NAME_ID_WWS_SUBFAMILY_NAME = 22,
  NAME_ID_LIGHT_BACKGROUND_PALETTE = 23,
  NAME_ID_DARK_BACKGROUND_PALETTE = 24,
  NAME_ID_VARIATIONS_POSTSCRIPT_NAME_PREFIX = 15
} NameId;

typedef enum {
  SIMPLE_GLYPH_FLAG_ON_CURVE_POINT = 0x01,
  SIMPLE_GLYPH_FLAG_X_SHORT_VECTOR = 0x02,
  SIMPLE_GLYPH_FLAG_Y_SHORT_VECTOR = 0x04,
  SIMPLE_GLYPH_FLAG_REPEAT_FLAG = 0x08,
  SIMPLE_GLYPH_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR = 0x10,
  SIMPLE_GLYPH_FLAG_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR = 0x20,
  SIMPLE_GLYPH_FLAG_OVERLAP_SIMPLE = 0x40
} SimpleGlyphFlag;

typedef enum {
  COMPOSITE_GLYPH_ARGS_WORDS = 0x0001,
  COMPOSITE_GLYPH_ARGS_XY = 0x0002,
  COMPOSITE_GLYPH_ROUND_XY_TO_GRID = 0x0004,
  COMPOSITE_GLYPH_HAS_SCALE = 0x0008,
  COMPOSITE_GLYPH_MORE_COMPONENTS = 0x0020,
  COMPOSITE_GLYPH_HAS_XY_SCALE = 0x0040,
  COMPOSITE_GLYPH_HAS_TRANSFORM = 0x0080,
  COMPOSITE_GLYPH_HAS_INSTRUCTIONS = 0x0100,
  COMPOSITE_GLYPH_USE_METRICS = 0x0200,
  COMPOSITE_GLYPH_OVERLAP_COMPOUND = 0x0400,
  COMPOSITE_GLYPH_SCALED_COMPONENT_OFFSET = 0x0800,
  COMPOSITE_GLYPH_UNSCALED_COMPONENT_OFFSET = 0x1000
} CompositeGlyphFlag;

typedef struct {
  UtObject object;

  // Data being decoded.
  UtObject *data;

  // Decoded glyphs.
  UtObject *glyphs;

  // Decoded font.
  UtObject *font;

  // First error that occurred during decoding.
  UtObject *error;
} UtOpenTypeDecoder;

static void set_error(UtOpenTypeDecoder *self, const char *description) {
  if (self->error != NULL) {
    return;
  }

  self->error = ut_open_type_error_new(description);
}

static void set_error_take(UtOpenTypeDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static uint8_t get_uint8(UtObject *data, size_t offset) {
  return ut_uint8_list_get_element(data, offset);
}

static int8_t get_int8(UtObject *data, size_t offset) {
  return ut_uint8_list_get_element(data, offset);
}

static uint16_t get_uint16(UtObject *data, size_t offset) {
  return ut_uint8_list_get_uint16_be(data, offset);
}

static int16_t get_int16(UtObject *data, size_t offset) {
  return ut_uint8_list_get_int16_be(data, offset);
}

static uint32_t get_uint32(UtObject *data, size_t offset) {
  return ut_uint8_list_get_uint32_be(data, offset);
}

static double get_float16(UtObject *data, size_t offset) {
  uint16_t value = get_uint16(data, offset);
  double integer = value >> 14;
  double fraction = (value & 0x3fff) / 16384.0;
  if (integer <= 1) {
    return integer + fraction;
  } else {
    return integer - 4 - fraction;
  }
}

static bool decode_font_header_table(UtOpenTypeDecoder *self, UtObject *table,
                                     uint16_t *units_per_em,
                                     int16_t *index_to_loc_format) {
  size_t table_length = ut_list_get_length(table);

  if (table_length < 54) {
    set_error(self, "Invalid length font header table");
    return false;
  }

  size_t offset = 0;
  uint16_t major_version = get_uint16(table, offset);
  uint16_t minor_version = get_uint16(table, offset + 2);
  // uint32_t font_revision = get_uint32(table, offset + 4);
  // uint32_t checksum_adjustment = get_uint32(table, offset + 8);
  uint32_t magic_number = get_uint32(table, offset + 12);
  // uint16_t flags = get_uint16(table, offset + 16);
  uint16_t units_per_em_ = get_uint16(table, offset + 18);
  // uint64_t created = get_uint64(table, offset + 20);
  // uint64_t modified = get_uint64(table, offset + 28);
  // int16_t x_min = get_int16(table, offset + 36);
  // int16_t y_min = get_int16(table, offset + 38);
  // int16_t x_max = get_int16(table, offset + 40);
  // int16_t y_max = get_int16(table, offset + 42);
  // uint16_t mac_style = get_uint16(table, offset + 44);
  // uint16_t lowest_rec_ppem = get_uint16(table, offset + 46);
  // int16_t font_direction_hint = get_int16(table, offset + 48);
  int16_t index_to_loc_format_ = get_int16(table, offset + 50);
  int16_t glyph_data_format = get_int16(table, offset + 52);

  if (major_version != FONT_HEADER_VERSION_MAJOR &&
      minor_version != FONT_HEADER_VERSION_MINOR) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid font header version %d.%d",
                                         major_version, minor_version));
    return false;
  }

  // FIXME: Support checksum.

  if (magic_number != 0x5F0F3CF5) {
    set_error(self, "Invalid font header magic number");
    return false;
  }

  if (index_to_loc_format_ < 0 || index_to_loc_format_ > 1) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid index to location format %d",
                                         index_to_loc_format_));
    return false;
  }

  if (glyph_data_format != GLYPH_DATA_FORMAT) {
    set_error_take(self,
                   ut_cstring_new_printf("Unsupported glyph data format %d",
                                         glyph_data_format));
    return false;
  }

  *units_per_em = units_per_em_;
  *index_to_loc_format = index_to_loc_format_;
  return true;
}

static bool decode_maximum_profile_table(UtOpenTypeDecoder *self,
                                         UtObject *table, size_t *n_glyphs) {
  size_t table_length = ut_list_get_length(table);

  if (table_length < 6) {
    set_error(self, "Invalid length maximum profile table");
    return false;
  }

  size_t offset = 0;
  uint32_t version = get_uint16(table, offset);
  uint16_t n_glyphs_ = get_uint16(table, offset + 4);
  if (version == 0x00000005) {
    if (table_length != 6) {
      set_error(self, "Invalid length maximum profile table");
      return false;
    }
    return true;
  } else if (version != 0x00000001) {
    set_error_take(self,
                   ut_cstring_new_printf(
                       "Invalid maximum profile table version %08x", version));
    return false;
  }

  if (table_length != 32) {
    set_error(self, "Invalid length maximum profile table");
    return false;
  }

  // uint16_t max_points = get_uint16(table, offset + 6);
  // uint16_t max_contours = get_uint16(table, offset + 8);
  // uint16_t max_composite_points = get_uint16(table, offset + 10);
  // uint16_t max_composite_contours = get_uint16(table, offset + 12);
  // uint16_t max_zones = get_uint16(table, offset + 14);
  // uint16_t max_twilight_points = get_uint16(table, offset + 16);
  // uint16_t max_storage = get_uint16(table, offset + 18);
  // uint16_t max_function_defs = get_uint16(table, offset + 20);
  // uint16_t max_instruction_defs = get_uint16(table, offset + 22);
  // uint16_t max_stack_elements = get_uint16(table, offset + 24);
  // uint16_t max_size_of_instructions = get_uint16(table, offset + 26);
  // uint16_t max_component_elements = get_uint16(table, offset + 28);
  // uint16_t max_component_depth = get_uint16(table, offset + 30);

  *n_glyphs = n_glyphs_;
  return true;
}

static char *decode_utf16_naming_string(UtObject *table, size_t start,
                                        size_t length) {
  UtObjectRef code_units = ut_uint16_list_new();
  for (size_t j = 0; j < length; j += 2) {
    ut_uint16_list_append(code_units, get_uint16(table, start + j));
  }
  UtObjectRef string = ut_string_new_from_utf16(code_units);
  return ut_string_take_text(string);
}

static char *decode_ascii_naming_string(UtObject *table, size_t start,
                                        size_t length) {
  UtObjectRef string_data = ut_list_get_sublist(table, start, length);
  UtObjectRef string = ut_string_new_from_ascii(string_data);
  return ut_string_take_text(string);
}

static char *decode_naming_string(uint16_t platform_id, uint16_t encoding_id,
                                  UtObject *table, size_t start,
                                  size_t length) {
  if (platform_id == PLATFORM_ID_UNICODE) {
    return decode_utf16_naming_string(table, start, length);
  } else if (platform_id == PLATFORM_ID_WINDOWS &&
             encoding_id == WINDOWS_PLATFORM_ENCODING_UNICODE_BMP) {
    return decode_utf16_naming_string(table, start, length);
  } else if (platform_id == PLATFORM_ID_MACINTOSH && encoding_id == 0) {
    return decode_ascii_naming_string(table, start, length);
  } else {
    return NULL;
  }
}

static bool decode_naming_table(UtOpenTypeDecoder *self, UtObject *table) {
  size_t table_length = ut_list_get_length(table);

  if (table_length < 6) {
    set_error(self, "Invalid length naming table");
    return false;
  }

  uint32_t version = get_uint16(table, 0);
  uint32_t count = get_uint16(table, 2);
  uint32_t storage_offset = get_uint16(table, 4);

  if (version != 0 && version != 1) {
    set_error_take(self, ut_cstring_new_printf(
                             "Unsupported naming table version %d", version));
  }

  if (table_length < 6 + 12 * count) {
    set_error(self, "Invalid length naming table");
    return false;
  }
  size_t offset = 6;
  for (size_t i = 0; i < count; i++) {
    uint16_t platform_id = get_uint16(table, offset);
    uint16_t encoding_id = get_uint16(table, offset + 2);
    // uint16_t name_id = get_uint16(table, offset + 6);
    uint16_t length = get_uint16(table, offset + 8);
    uint16_t string_offset = get_uint16(table, offset + 10);

    size_t start = storage_offset + string_offset;
    if (start + length > table_length) {
      set_error(self, "Invalid naming table entry");
      return false;
    }

    ut_cstring_ref string =
        decode_naming_string(platform_id, encoding_id, table, start, length);
    if (string != NULL) {
      // FIXMEprintf("%d=%s\n", name_id, string);
    }

    offset += 12;
  }

  if (version == 1) {
    if (offset + 2 > table_length) {
      set_error(self, "Invalid length naming table");
      return false;
    }
    uint16_t lang_tag_count = get_uint16(table, offset);
    offset += 2;
    if (offset + 4 * lang_tag_count > table_length) {
      set_error(self, "Invalid length naming table");
      return false;
    }
    for (size_t i = 0; i < lang_tag_count; i++) {
      uint16_t length = get_uint16(table, offset);
      uint16_t lang_tag_offset = get_uint16(table, offset + 2);
      size_t start = storage_offset + lang_tag_offset;
      if (start + length > table_length) {
        set_error(self, "Invalid naming table language entry");
        return false;
      }

      ut_cstring_ref lang_tag =
          decode_utf16_naming_string(table, start, length);

      offset += 4;
    }
  }

  return true;
}

static bool decode_windows_bmp_character_map(UtOpenTypeDecoder *self,
                                             UtObject *map) {
  size_t map_length = ut_list_get_length(map);

  size_t offset = 0;
  uint16_t format = get_uint16(map, offset);
  if (format != 4) {
    set_error(self, "Invalid Windows BMP character map format");
    return false;
  }
  if (map_length < 14) {
    set_error(self, "Invalid Windows BMP character map length");
    return false;
  }
  // uint16_t length = get_uint16(map, offset + 2);
  // uint16_t language = get_uint16(map, offset + 4);
  uint16_t segment_count_x2 = get_uint16(map, offset + 6);
  // uint16_t search_range = get_uint16(map, offset + 8);
  // uint16_t entry_selector = get_uint16(map, offset + 10);
  // uint16_t range_shift = get_uint16(map, offset + 12);

  assert(segment_count_x2 % 2 == 0);
  size_t segment_count = segment_count_x2 / 2;

  if (map_length < 16 + segment_count * 8) {
    set_error(self, "Invalid Windows BMP character map length");
    return false;
  }

  size_t end_code_offset = offset + 14;
  offset = end_code_offset + segment_count * 2;
  // uint16_t reserved_pad = get_uint16(map, offset);
  size_t start_code_offset = offset + 2;
  size_t id_delta_offset = start_code_offset + segment_count * 2;
  size_t id_range_offsets_offset = id_delta_offset + segment_count * 2;
  // size_t glyph_id_array_offset = id_range_offsets_offset + segment_count * 2;
  size_t glyph_id_array_length = map_length - offset;

  for (size_t i = 0; i < segment_count; i++) {
    // uint16_t start_code = get_uint16(map, start_code_offset + i * 2);
    // uint16_t end_code = get_uint16(map, end_code_offset + i * 2);
    // int16_t id_delta = get_int16(map, id_delta_offset + i * 2);
    uint16_t id_range_offset = get_uint16(map, id_range_offsets_offset + i * 2);
    if (id_range_offset + 2 > glyph_id_array_length) {
      set_error(self, "Invalid Windows BMP character map glyph id");
      return false;
    }
    // uint16_t glyph_index =
    //     id_range_offset != 0
    //         ? get_int16(map, glyph_id_array_offset + id_range_offset * 2)
    //         : 0;
    //  printf("%d-%d => %d-%d\n", start_code, end_code,
    //         start_code + id_delta + glyph_index,
    //         end_code + id_delta + glyph_index);
  }

  return true;
}

static bool decode_character_map(UtOpenTypeDecoder *self, UtObject *table) {
  size_t table_length = ut_list_get_length(table);

  size_t offset = 0;
  if (table_length < 4) {
    set_error(self, "Invalid length character map table");
    return false;
  }
  uint16_t version = get_uint16(table, offset);
  if (version != 0) {
    set_error(self, "Invalid character map table version");
    return false;
  }
  uint16_t n_tables = get_uint16(table, offset + 2);
  if (table_length < 4 + n_tables * 8) {
    set_error(self, "Invalid length character map table");
    return false;
  }
  offset += 4;

  UtObjectRef windows_bmp_character_map = NULL;
  for (size_t i = 0; i < n_tables; i++) {
    uint16_t platform_id = get_uint16(table, offset);
    uint16_t encoding_id = get_uint16(table, offset + 2);
    size_t map_offset = get_uint32(table, offset + 4);
    if (map_offset + 4 > table_length) {
      set_error(self, "Invalid character map subtable");
      return false;
    }
    uint16_t map_format = get_uint16(table, map_offset);
    size_t map_length = 2;
    switch (map_format) {
    case 0:
    case 2:
    case 4:
    case 6:
      map_length = get_uint16(table, map_offset + 2);
      break;
    case 8:
    case 10:
    case 12:
    case 13:
      if (map_offset + 8 > table_length) {
        set_error(self, "Invalid character map subtable");
        return false;
      }
      map_length = get_uint32(table, map_offset + 4);
      break;
    case 14:
      if (map_offset + 6 > table_length) {
        set_error(self, "Invalid character map subtable");
        return false;
      }
      map_length = get_uint32(table, map_offset + 2);
      break;
    }
    if (map_offset + map_length > table_length) {
      set_error(self, "Invalid character map subtable");
      return false;
    }

    offset += 8;

    UtObjectRef map = ut_list_get_sublist(table, map_offset, map_length);

    if (platform_id == PLATFORM_ID_WINDOWS &&
        encoding_id == WINDOWS_PLATFORM_ENCODING_UNICODE_BMP &&
        windows_bmp_character_map == NULL) {
      windows_bmp_character_map = ut_object_ref(map);
    }
  }

  if (windows_bmp_character_map != NULL) {
    if (!decode_windows_bmp_character_map(self, windows_bmp_character_map)) {
      return false;
    }
  }

  return true;
}

static bool decode_horizontal_header(UtOpenTypeDecoder *self, UtObject *table,
                                     size_t *n_horizontal_metrics) {
  size_t table_length = ut_list_get_length(table);

  if (table_length < 4) {
    set_error(self, "Invalid length horizontal header table");
    return false;
  }

  uint16_t major_version = get_uint16(table, 0);
  uint16_t minor_version = get_uint16(table, 2);
  if (major_version != HORIZONTAL_HEADER_VERSION_MAJOR &&
      minor_version != HORIZONTAL_HEADER_VERSION_MINOR) {
    set_error_take(
        self, ut_cstring_new_printf("Invalid horizontal header version %d.%d",
                                    major_version, minor_version));
    return false;
  }

  if (table_length != 36) {
    set_error(self, "Invalid length horizontal header table");
    return false;
  }

  // uint16_t ascender = get_uint16(table, 4);
  // uint16_t descender = get_uint16(table, 6);
  // uint16_t line_gap = get_uint16(table, 8);
  // uint16_t advance_width_max = get_uint16(table, 10);
  // uint16_t min_left_side_bearing = get_uint16(table, 12);
  // uint16_t min_right_side_bearing = get_uint16(table, 14);
  // uint16_t x_max_extent = get_uint16(table, 16);
  // uint16_t caret_slope_rise = get_int16(table, 18);
  // uint16_t caret_slope_run = get_int16(table, 20);
  // uint16_t caret_offset = get_int16(table, 22);
  int16_t metric_data_format = get_int16(table, 32);
  uint16_t n_horizontal_metrics_ = get_uint16(table, 34);

  if (metric_data_format != METRIC_DATA_FORMAT) {
    set_error_take(self,
                   ut_cstring_new_printf("Unsupported metric data format %d",
                                         metric_data_format));
    return false;
  }

  *n_horizontal_metrics = n_horizontal_metrics_;
  return true;
}

static void get_horizontal_metrics(UtObject *table, size_t n_glyphs,
                                   size_t n_horizontal_metrics, size_t index,
                                   uint16_t *advance_width,
                                   int16_t *left_side_bearing) {
  if (index < n_horizontal_metrics) {
    size_t offset = index * 4;
    *advance_width = get_uint16(table, offset);
    *left_side_bearing = get_int16(table, offset + 2);
  } else {
    size_t offset =
        n_horizontal_metrics * 4 + (index - n_horizontal_metrics) * 2;
    *advance_width = 0;
    *left_side_bearing = get_int16(table, offset);
  }
}

static UtObject *decode_simple_glyph(UtOpenTypeDecoder *self, size_t n_contours,
                                     int16_t x_min, int16_t y_min,
                                     int16_t x_max, int16_t y_max,
                                     uint16_t advance_width,
                                     int16_t left_side_bearing,
                                     UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t offset = 10;

  if (data_length < offset + (n_contours * 2) + 2) {
    set_error(self, "Insufficient space for glyph end points");
    return NULL;
  }
  uint64_t contour_end[n_contours];
  for (size_t i = 0; i < n_contours; i++) {
    contour_end[i] = get_uint16(data, offset);
    offset += 2;
  }
  size_t n_points = n_contours > 0 ? contour_end[n_contours - 1] + 1 : 0;
  size_t instructions_length = get_uint16(data, offset);
  offset += 2;
  if (data_length < offset + instructions_length) {
    set_error(self, "Insufficient space for glyph instuctions");
    return NULL;
  }
  UtObjectRef instructions =
      ut_list_get_sublist(data, offset, instructions_length);
  offset += instructions_length;

  // Expand flags.
  UtObjectRef flags = ut_uint8_list_new();
  while (ut_list_get_length(flags) < n_points) {
    if (data_length < offset) {
      set_error(self, "Insufficient space for glyph flags");
    }
    uint8_t flag = get_uint8(data, offset);
    offset++;
    size_t count = 1;
    if ((flag & SIMPLE_GLYPH_FLAG_REPEAT_FLAG) != 0) {
      if (data_length < offset) {
        set_error(self, "Insufficient space for glyph flags");
      }
      count += get_uint8(data, offset);
      offset++;
    }
    for (size_t i = 0; i < count; i++) {
      ut_uint8_list_append(flags, flag);
    }
  }
  if (ut_list_get_length(flags) != n_points) {
    set_error(self, "Invalid flags in glyph data");
    return NULL;
  }

  UtObjectRef coords = ut_int16_array_new_sized(n_points * 2);
  int16_t *coords_array = ut_int16_list_get_writable_data(coords);

  // Read x co-ordinates.
  int16_t x = 0;
  for (size_t i = 0; i < n_points; i++) {
    size_t flag = ut_uint8_list_get_element(flags, i);
    if ((flag & SIMPLE_GLYPH_FLAG_X_SHORT_VECTOR) != 0) {
      if (data_length < offset + 1) {
        set_error(self, "Insufficient space for glyph co-ordinates");
      }
      offset++;
      uint8_t dx = get_uint8(data, offset);
      if ((flag & SIMPLE_GLYPH_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) !=
          0) {
        x += dx;
      } else {
        x -= dx;
      }
    } else {
      int16_t dx = 0;
      if ((flag & SIMPLE_GLYPH_FLAG_X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR) ==
          0) {
        if (data_length < offset + 2) {
          set_error(self, "Insufficient space for glyph co-ordinates");
        }
        dx = get_int16(data, offset);
        offset += 2;
      }
      x += dx;
    }
    coords_array[i * 2] = x;
  }

  // Read y co-ordinates.
  int16_t y = 0;
  for (size_t i = 0; i < n_points; i++) {
    size_t flag = ut_uint8_list_get_element(flags, i);
    if ((flag & SIMPLE_GLYPH_FLAG_Y_SHORT_VECTOR) != 0) {
      if (data_length < offset + 1) {
        set_error(self, "Insufficient space for glyph co-ordinates");
      }
      offset++;
      uint8_t dy = get_uint8(data, offset);
      if ((flag & SIMPLE_GLYPH_FLAG_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) !=
          0) {
        y += dy;
      } else {
        y -= dy;
      }
    } else {
      int16_t dy = 0;
      if ((flag & SIMPLE_GLYPH_FLAG_Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR) ==
          0) {
        if (data_length < offset + 2) {
          set_error(self, "Insufficient space for glyph co-ordinates");
        }
        dy = get_int16(data, offset);
        offset += 2;
      }
      y += dy;
    }
    coords_array[i * 2 + 1] = y;
  }

  UtObjectRef contours = ut_list_new();
  size_t start = 0;
  for (size_t i = 0; i < n_contours; i++) {
    size_t end = contour_end[i];
    ut_list_append_take(contours,
                        ut_list_get_sublist(coords, start, end - start + 1));
    start = end + 1;
  }

  return ut_true_type_simple_glyph_new(
      x_min, y_min, x_max, y_max, advance_width, left_side_bearing, contours);
}

static UtObject *decode_composite_glyph(UtOpenTypeDecoder *self, int16_t x_min,
                                        int16_t y_min, int16_t x_max,
                                        int16_t y_max, uint16_t advance_width,
                                        int16_t left_side_bearing,
                                        UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t offset = 10;

  UtObjectRef components = ut_list_new();
  bool has_instructions = false;
  while (true) {
    if (offset + 4 > data_length) {
      set_error(self,
                "Insufficient space for composite glyph component header");
      return NULL;
    }

    uint16_t flags = get_uint16(data, offset);
    uint16_t index = get_uint16(data, offset + 2);

    size_t component_length = 4;
    if ((flags & COMPOSITE_GLYPH_ARGS_WORDS) != 0) {
      component_length += 4;
    } else {
      component_length += 2;
    }
    if ((flags & COMPOSITE_GLYPH_HAS_SCALE) != 0) {
      component_length += 2;
    } else if ((flags & COMPOSITE_GLYPH_HAS_XY_SCALE) != 0) {
      component_length += 4;
    } else if ((flags & COMPOSITE_GLYPH_HAS_TRANSFORM) != 0) {
      component_length += 8;
    }
    if (offset + component_length > data_length) {
      set_error(self, "Insufficient space for composite glyph component");
      return NULL;
    }

    offset += 4;
    int16_t x_offset = 0, y_offset = 0;
    if ((flags & COMPOSITE_GLYPH_ARGS_XY) != 0) {
      if ((flags & COMPOSITE_GLYPH_ARGS_WORDS) != 0) {
        x_offset = get_int16(data, offset);
        y_offset = get_int16(data, offset + 2);
        offset += 4;
      } else {
        // FIXME: Signed or unsigned?
        x_offset = get_int8(data, offset);
        y_offset = get_int8(data, offset + 1);
        offset += 2;
      }
    } else {
      if ((flags & COMPOSITE_GLYPH_ARGS_WORDS) != 0) {
        get_uint16(data, offset);     // point1
        get_uint16(data, offset + 2); // point2
        offset += 4;
      } else {
        get_uint8(data, offset);     // point1
        get_uint8(data, offset + 1); // point2
        offset += 2;
      }
    }
    double transform[4] = {1.0, 0.0, 0.0, 1.0};
    if ((flags & COMPOSITE_GLYPH_HAS_SCALE) != 0) {
      transform[0] = transform[3] = get_float16(data, offset);
      offset += 2;
    } else if ((flags & COMPOSITE_GLYPH_HAS_XY_SCALE) != 0) {
      transform[0] = get_float16(data, offset);
      transform[3] = get_float16(data, offset + 2);
      offset += 4;
    } else if ((flags & COMPOSITE_GLYPH_HAS_TRANSFORM) != 0) {
      transform[0] = get_float16(data, offset);
      transform[1] = get_float16(data, offset + 2);
      transform[2] = get_float16(data, offset + 4);
      transform[3] = get_float16(data, offset + 6);
      offset += 8;
    }

    ut_list_append_take(components, ut_true_type_composite_glyph_component_new(
                                        x_offset, y_offset, index));

    if ((flags & COMPOSITE_GLYPH_MORE_COMPONENTS) == 0) {
      has_instructions = (flags & COMPOSITE_GLYPH_HAS_INSTRUCTIONS) != 0;
      break;
    }
  }

  UtObjectRef instructions = NULL;
  if (has_instructions) {
    if (offset + 2 > data_length) {
      set_error(self,
                "Insufficient space for composite glyph instructions length");
      return NULL;
    }
    uint16_t instructions_length = get_uint16(data, offset);
    offset += 2;
    if (offset + instructions_length > data_length) {
      set_error(self, "Insufficient space for composite glyph instructions");
      return NULL;
    }

    instructions = ut_list_get_sublist(data, offset, instructions_length);
    offset += instructions_length;
  }

  return ut_true_type_composite_glyph_new(
      x_min, y_min, x_max, y_max, advance_width, left_side_bearing, components);
}

static UtObject *decode_glyph(UtOpenTypeDecoder *self, uint16_t advance_width,
                              int16_t left_side_bearing, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 10) {
    set_error(self, "Insufficient space for glyph data header");
    return NULL;
  }

  int16_t n_contours = get_int16(data, 0);
  int16_t x_min = get_int16(data, 2);
  int16_t y_min = get_int16(data, 4);
  int16_t x_max = get_int16(data, 6);
  int16_t y_max = get_int16(data, 8);

  if (n_contours >= 0) {
    return decode_simple_glyph(self, n_contours, x_min, y_min, x_max, y_max,
                               advance_width, left_side_bearing, data);
  } else {
    return decode_composite_glyph(self, x_min, y_min, x_max, y_max,
                                  advance_width, left_side_bearing, data);
  }
}

static size_t get_glyph_offset(UtObject *index_to_location_table,
                               int16_t index_to_loc_format, size_t index) {
  if (index_to_loc_format == 0) {
    return get_uint16(index_to_location_table, index * 2) * 2;
  } else {
    return get_uint32(index_to_location_table, index * 4);
  }
}

static UtObject *decode_glyph_data(UtOpenTypeDecoder *self, size_t n_glyphs,
                                   UtObject *index_to_location_table,
                                   int16_t index_to_loc_format,
                                   UtObject *horizontal_header_table,
                                   UtObject *horizontal_metrics_table,
                                   UtObject *table) {
  size_t table_length = ut_list_get_length(table);

  size_t n_horizontal_metrics;
  if (!decode_horizontal_header(self, horizontal_header_table,
                                &n_horizontal_metrics)) {
    return NULL;
  }

  size_t expected_index_to_location_table_length;
  if (index_to_loc_format == 0) {
    expected_index_to_location_table_length = (n_glyphs + 1) * 2;
  } else {
    expected_index_to_location_table_length = (n_glyphs + 1) * 4;
  }
  if (ut_list_get_length(index_to_location_table) !=
      expected_index_to_location_table_length) {
    set_error(self, "Invalid length index to location table");
    return NULL;
  }

  UtObjectRef glyphs = ut_list_new();
  size_t start_offset =
      get_glyph_offset(index_to_location_table, index_to_loc_format, 0);
  for (size_t i = 0; i < n_glyphs; i++) {
    uint16_t advance_width;
    int16_t left_side_bearing;
    get_horizontal_metrics(horizontal_metrics_table, n_glyphs,
                           n_horizontal_metrics, i, &advance_width,
                           &left_side_bearing);

    size_t end_offset =
        get_glyph_offset(index_to_location_table, index_to_loc_format, i + 1);
    if (start_offset > table_length || end_offset > table_length ||
        end_offset < start_offset) {
      set_error_take(self, "Invalid index to location offsets");
      return NULL;
    }
    size_t glyph_data_length = end_offset - start_offset;

    if (glyph_data_length > 0) {
      UtObjectRef glyph_data =
          ut_list_get_sublist(table, start_offset, glyph_data_length);
      UtObjectRef glyph =
          decode_glyph(self, advance_width, left_side_bearing, glyph_data);
      if (glyph == NULL) {
        return NULL;
      }
      ut_list_append(glyphs, glyph);
    } else {
      ut_list_append(glyphs, NULL);
    }

    start_offset = end_offset;
  }

  return ut_object_ref(glyphs);
}

static UtObject *decode(UtOpenTypeDecoder *self) {
  size_t data_length = ut_list_get_length(self->data);
  if (data_length < 12) {
    set_error(self, "Invalid space for table directory");
    return NULL;
  }

  uint32_t sfnt_version = get_uint32(self->data, 0);
  if (sfnt_version != VERSION_OPEN_TYPE) {
    set_error(self, "Unsupported OpenType version");
    return NULL;
  }

  uint16_t n_tables = get_uint16(self->data, 4);
  // uint16_t search_range = get_uint16(self->data, 6);
  // uint16_t entry_selector = get_uint16(self->data, 8);
  // uint16_t range_shift = get_uint16(self->data, 10);

  size_t directory_length = 12 + n_tables * 16;
  if (data_length < directory_length) {
    set_error(self, "Invalid space for table directory");
    return NULL;
  }

  UtObjectRef character_to_glyph_index_mapping_table = NULL;
  UtObjectRef glyph_data_table = NULL;
  UtObjectRef font_header_table = NULL;
  UtObjectRef horizontal_header_table = NULL;
  UtObjectRef horizontal_metrics_table = NULL;
  UtObjectRef index_to_location_table = NULL;
  UtObjectRef maximum_profile_table = NULL;
  UtObjectRef naming_table = NULL;
  UtObjectRef postscript_table = NULL;
  size_t offset = 12;
  for (size_t i = 0; i < n_tables; i++) {
    uint32_t tag = get_uint32(self->data, offset);
    // uint32_t checksum = get_uint32(self->data, offset + 4);
    size_t table_offset = get_uint32(self->data, offset + 8);
    size_t table_length = get_uint32(self->data, offset + 12);
    if (table_offset >= data_length ||
        (table_offset + table_length) > data_length) {
      set_error_take(self,
                     ut_cstring_new_printf(
                         "Invalid %c%c%c%c table (offset %zi, length %zi) "
                         "in table directory",
                         tag >> 24, (tag >> 16) & 0xff, (tag >> 8) & 0xff,
                         tag & 0xff, table_offset, table_length));
    }
    UtObjectRef table =
        ut_list_get_sublist(self->data, table_offset, table_length);
    offset += 16;

    // FIXME: Checksum

    switch (tag) {
    case TABLE_CHARACTER_TO_GLYPH_INDEX_MAPPING:
      if (character_to_glyph_index_mapping_table == NULL) {
        character_to_glyph_index_mapping_table = ut_object_ref(table);
      }
      break;
    case TABLE_GLYPH_DATA:
      if (glyph_data_table == NULL) {
        glyph_data_table = ut_object_ref(table);
      }
      break;
    case TABLE_FONT_HEADER:
      if (font_header_table == NULL) {
        font_header_table = ut_object_ref(table);
      }
      break;
    case TABLE_HORIZONTAL_HEADER:
      if (horizontal_header_table == NULL) {
        horizontal_header_table = ut_object_ref(table);
      }
      break;
    case TABLE_HORIZONTAL_METRICS:
      if (horizontal_metrics_table == NULL) {
        horizontal_metrics_table = ut_object_ref(table);
      }
      break;
    case TABLE_INDEX_TO_LOCATION:
      if (index_to_location_table == NULL) {
        index_to_location_table = ut_object_ref(table);
      }
      break;
    case TABLE_MAXIMUM_PROFILE:
      if (maximum_profile_table == NULL) {
        maximum_profile_table = ut_object_ref(table);
      }
      break;
    case TABLE_NAMING:
      if (naming_table == NULL) {
        naming_table = ut_object_ref(table);
      }
      break;
    case TABLE_POSTSCRIPT:
      if (postscript_table == NULL) {
        postscript_table = ut_object_ref(table);
      }
      break;
    default:
      break;
    }
  }

  if (font_header_table == NULL) {
    set_error(self, "Missing required font header table");
    return NULL;
  }
  int16_t index_to_loc_format;
  uint16_t units_per_em;
  if (!decode_font_header_table(self, font_header_table, &units_per_em,
                                &index_to_loc_format)) {
    return NULL;
  }

  if (maximum_profile_table == NULL) {
    set_error(self, "Missing required maximum profile table");
    return NULL;
  }
  size_t n_glyphs;
  if (!decode_maximum_profile_table(self, maximum_profile_table, &n_glyphs)) {
    return NULL;
  }

  if (naming_table == NULL) {
    set_error(self, "Missing required naming table");
    return NULL;
  }
  if (!decode_naming_table(self, naming_table)) {
    return NULL;
  }

  if (character_to_glyph_index_mapping_table == NULL) {
    set_error(self, "Missing required character map table");
    return NULL;
  }
  if (!decode_character_map(self, character_to_glyph_index_mapping_table)) {
    return NULL;
  }

  if (index_to_location_table == NULL) {
    set_error(self, "Missing index to location table");
    return NULL;
  }
  // FIXME: Validate index_to_location_table length
  if (horizontal_header_table == NULL) {
    set_error(self, "Missing horizontal header table");
    return NULL;
  }
  if (horizontal_metrics_table == NULL) {
    set_error(self, "Missing horizontal metrics table");
    return NULL;
  }
  // FIXME: Validate horizontal_metrics_table length
  // if (ut_list_get_lengtht(horizontal_metrics_table) != (4 *
  // n_horizontal_metrics) + 2 * (n_glyphs - n_horizontal_metrics)) {
  //   set_error(self, "Invalid length horizontal header table");
  //   return false;
  // }
  if (glyph_data_table == NULL) {
    set_error(self, "Missing glyph data table");
    return NULL;
  }
  UtObjectRef glyphs = decode_glyph_data(
      self, n_glyphs, index_to_location_table, index_to_loc_format,
      horizontal_header_table, horizontal_metrics_table, glyph_data_table);
  if (glyphs == NULL) {
    return NULL;
  }

  return ut_open_type_font_new(units_per_em, glyphs);
}

static void ut_open_type_decoder_cleanup(UtObject *object) {
  UtOpenTypeDecoder *self = (UtOpenTypeDecoder *)object;
  ut_object_unref(self->data);
  ut_object_unref(self->glyphs);
  ut_object_unref(self->font);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtOpenTypeDecoder",
                                             .cleanup =
                                                 ut_open_type_decoder_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *ut_open_type_decoder_new(UtObject *data) {
  UtObject *object =
      ut_object_new(sizeof(UtOpenTypeDecoder), &object_interface);
  UtOpenTypeDecoder *self = (UtOpenTypeDecoder *)object;
  self->data = ut_object_ref(data);
  return object;
}

void ut_open_type_decoder_decode(UtObject *object) {
  assert(ut_object_is_open_type_decoder(object));
  UtOpenTypeDecoder *self = (UtOpenTypeDecoder *)object;

  assert(self->font == NULL);
  self->font = decode(self);
}

UtObject *ut_open_type_decoder_get_font(UtObject *object) {
  assert(ut_object_is_open_type_decoder(object));
  UtOpenTypeDecoder *self = (UtOpenTypeDecoder *)object;
  return self->font;
}

UtObject *ut_open_type_decoder_get_error(UtObject *object) {
  assert(ut_object_is_open_type_decoder(object));
  UtOpenTypeDecoder *self = (UtOpenTypeDecoder *)object;
  return self->error;
}

bool ut_object_is_open_type_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
