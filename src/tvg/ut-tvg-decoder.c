#include <assert.h>

#include "ut-tvg.h"
#include "ut.h"

// https://tinyvg.tech/

// FIXME: Carefully handle counts overflowing with the +1s

typedef enum {
  DECODER_STATE_HEADER,
  DECODER_STATE_COLOR_TABLE,
  DECODER_STATE_COMMAND,
  DECODER_STATE_ERROR,
  DECODER_STATE_END
} DecoderState;

typedef struct {
  UtObject object;

  // Input stream being read.
  UtObject *input_stream;

  // Callback to notify when complete.
  UtObject *callback_object;
  UtTvgDecodeCallback callback;

  // Current state of the decoder.
  DecoderState state;

  // Width of image in display units.
  uint32_t width;

  // Height of image in display units.
  uint32_t height;

  // Color encoding used in color table.
  UtTvgColorEncoding color_encoding;

  // Number of colors in color table.
  uint32_t color_count;

  // The color table.
  UtObject *colors;

  // Size of co-ordinates.
  UtTvgCoordinateRange coordinate_range;

  // Value to scale units by.
  float scale;

  // Draw commands.
  UtObject *commands;

  // Final image object.
  UtObject *image;

  // Error that occurred during decoding.
  UtObject *error;
} UtTvgDecoder;

static void notify_complete(UtTvgDecoder *self) {
  ut_input_stream_close(self->input_stream);
  if (self->callback_object != NULL) {
    self->callback(self->callback_object);
  }
}

static void set_error(UtTvgDecoder *self, const char *description) {
  if (self->state == DECODER_STATE_ERROR) {
    return;
  }

  self->error = ut_tvg_error_new(description);
  self->state = DECODER_STATE_ERROR;
  notify_complete(self);
}

static void set_error_take(UtTvgDecoder *self, char *description) {
  set_error(self, description);
  free(description);
}

static UtObject *lookup_color(UtTvgDecoder *self, size_t index) {
  if (index >= self->color_count) {
    set_error_take(self,
                   ut_cstring_new_printf("Invalid color index %zi", index));
    return NULL;
  }

  return ut_object_list_get_element(self->colors, index);
}

static size_t decode_varuint(UtTvgDecoder *self, UtObject *data, size_t *offset,
                             uint32_t *value) {
  size_t data_length = ut_list_get_length(data);

  uint32_t value_ = 0;
  size_t offset_ = *offset;
  size_t shift = 0;
  while (true) {
    if (data_length < offset_) {
      return false;
    }

    uint32_t v = ut_uint8_list_get_element(data, offset_);
    offset_++;

    value_ |= (v & 0x7f) << shift;
    shift += 7;

    if ((shift == 28 && (v & 0xf0) != 0) || shift > 28) {
      set_error(self, "Invalid length VarUInt");
      return false;
    }

    if ((v & 0x80) == 0) {
      *offset = offset_;
      *value = value_;
      return true;
    }
  }
}

static bool decode_default_unit(UtTvgDecoder *self, UtObject *data,
                                size_t *offset, float *value) {
  size_t data_length = ut_list_get_length(data);
  if (*offset + 2 > data_length) {
    return false;
  }

  int16_t v = ut_uint8_list_get_int16_le(data, *offset);
  (*offset) += 2;

  *value = v * self->scale;
  return true;
}

static bool decode_reduced_unit(UtTvgDecoder *self, UtObject *data,
                                size_t *offset, float *value) {
  size_t data_length = ut_list_get_length(data);
  if (*offset + 1 > data_length) {
    return false;
  }

  int8_t v = ut_uint8_list_get_int8(data, *offset);
  (*offset)++;

  *value = v * self->scale;
  return true;
}

static bool decode_enhanced_unit(UtTvgDecoder *self, UtObject *data,
                                 size_t *offset, float *value) {
  size_t data_length = ut_list_get_length(data);
  if (*offset + 4 > data_length) {
    return false;
  }

  int32_t v = ut_uint8_list_get_int32_le(data, *offset);
  (*offset) += 4;

  *value = v * self->scale;
  return true;
}

static bool decode_unit(UtTvgDecoder *self, UtObject *data, size_t *offset,
                        float *value) {
  switch (self->coordinate_range) {
  case UT_TVG_COORDINATE_RANGE_DEFAULT:
    return decode_default_unit(self, data, offset, value);
  case UT_TVG_COORDINATE_RANGE_REDUCED:
    return decode_reduced_unit(self, data, offset, value);
  case UT_TVG_COORDINATE_RANGE_ENHANCED:
    return decode_enhanced_unit(self, data, offset, value);
  default:
    return false;
  }
}

static bool decode_point(UtTvgDecoder *self, UtObject *data, size_t *offset,
                         float *x, float *y) {
  size_t o = *offset;
  float x_, y_;
  if (!decode_unit(self, data, &o, &x_) || !decode_unit(self, data, &o, &y_)) {
    return false;
  }

  *offset = o;
  *x = x_;
  *y = y_;
  return true;
}

static bool decode_path_line_command(UtTvgDecoder *self, UtObject *data,
                                     size_t *offset, float line_width,
                                     UtObject **value) {
  float x, y;
  if (!decode_point(self, data, offset, &x, &y)) {
    return false;
  }

  *value = ut_tvg_path_line_new(line_width, x, y);
  return true;
}

static bool decode_path_horizontal_line_command(UtTvgDecoder *self,
                                                UtObject *data, size_t *offset,
                                                float line_width,
                                                UtObject **value) {
  float x;
  if (!decode_unit(self, data, offset, &x)) {
    return false;
  }

  *value = ut_tvg_path_line_new(line_width, x, 0);
  return true;
}

static bool decode_path_vertical_line_command(UtTvgDecoder *self,
                                              UtObject *data, size_t *offset,
                                              float line_width,
                                              UtObject **value) {
  float y;
  if (!decode_unit(self, data, offset, &y)) {
    return false;
  }

  *value = ut_tvg_path_line_new(line_width, 0, y);
  return true;
}

static bool decode_path_cubic_bezier_command(UtTvgDecoder *self, UtObject *data,
                                             size_t *offset, float line_width,
                                             UtObject **value) {
  size_t o = *offset;

  float control_x0, control_y0, control_x1, control_y1, x1, y1;
  if (!decode_point(self, data, &o, &control_x0, &control_y0) ||
      !decode_point(self, data, &o, &control_x1, &control_y1) ||
      !decode_point(self, data, &o, &x1, &y1)) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_path_bezier_new(line_width, control_x0, control_y0,
                                  control_x1, control_y1, x1, y1);
  return true;
}

static bool decode_path_arc_circle_command(UtTvgDecoder *self, UtObject *data,
                                           size_t *offset, float line_width,
                                           UtObject **value) {
  size_t data_length = ut_list_get_length(data);
  size_t o = *offset;

  if (data_length < o) {
    return false;
  }
  // uint8_t flags = ut_uint8_list_get_element(data, o);
  o++;
  // bool large_arc = (flags & 0x1) != 0;
  // bool sweep_left = (flags & 0x2) != 0;

  float radius, x, y;
  if (!decode_unit(self, data, &o, &radius) ||
      !decode_point(self, data, &o, &x, &y)) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_path_arc_new_circle(line_width, radius, x, y);
  return true;
}

static bool decode_path_arc_ellipse_command(UtTvgDecoder *self, UtObject *data,
                                            size_t *offset, float line_width,
                                            UtObject **value) {
  size_t data_length = ut_list_get_length(data);
  size_t o = *offset;

  if (data_length < o) {
    return false;
  }
  // uint8_t flags = ut_uint8_list_get_element(data, o);
  o++;
  // bool large_arc = (flags & 0x1) != 0;
  // bool sweep_left = (flags & 0x2) != 0;

  float radius_x, radius_y, rotation, x, y;
  if (!decode_unit(self, data, &o, &radius_x) ||
      !decode_unit(self, data, &o, &radius_y) ||
      !decode_unit(self, data, &o, &rotation) ||
      !decode_point(self, data, &o, &x, &y)) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_path_arc_new(line_width, radius_x, radius_y, rotation, x, y);
  return true;
}

static bool decode_path_close_path_command(UtTvgDecoder *self, UtObject *data,
                                           size_t *offset, float x, float y,
                                           float line_width, UtObject **value) {
  *value = ut_tvg_path_line_new(line_width, x, y);
  return true;
}

static bool decode_path_quadratic_bezier_command(UtTvgDecoder *self,
                                                 UtObject *data, size_t *offset,
                                                 float line_width,
                                                 UtObject **value) {
  size_t o = *offset;

  float control_x, control_y, x1, y1;
  if (!decode_point(self, data, &o, &control_x, &control_y) ||
      !decode_point(self, data, &o, &x1, &y1)) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_path_bezier_new_quadratic(line_width, control_x, control_y,
                                            x1, y1);
  return true;
}

static bool decode_path_command(UtTvgDecoder *self, UtObject *data,
                                size_t *offset, float x, float y,
                                UtObject **value) {
  size_t data_length = ut_list_get_length(data);
  size_t o = *offset;

  if (data_length < o) {
    return false;
  }
  uint8_t tag = ut_uint8_list_get_element(data, o);
  o++;
  UtTvgInstruction instruction = tag & 0x7;
  bool has_line_width = ((tag >> 4) & 0x1) != 0;

  float line_width = 0;
  if (has_line_width) {
    if (!decode_unit(self, data, &o, &line_width)) {
      return false;
    }
  }

  bool decoded;
  switch (instruction) {
  case UT_TVG_INSTRUCTION_LINE:
    decoded = decode_path_line_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_HORIZONTAL_LINE:
    decoded =
        decode_path_horizontal_line_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_VERTICAL_LINE:
    decoded =
        decode_path_vertical_line_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_CUBIC_BEZIER:
    decoded =
        decode_path_cubic_bezier_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_ARC_CIRCLE:
    decoded = decode_path_arc_circle_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_ARC_ELLIPSE:
    decoded =
        decode_path_arc_ellipse_command(self, data, &o, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_CLOSE_PATH:
    decoded =
        decode_path_close_path_command(self, data, &o, x, y, line_width, value);
    break;
  case UT_TVG_INSTRUCTION_QUADRATIC_BEZIER:
    decoded =
        decode_path_quadratic_bezier_command(self, data, &o, line_width, value);
    break;
  default:
    assert(false);
  }

  if (decoded) {
    *offset = o;
  }
  return decoded;
}

static bool decode_path(UtTvgDecoder *self, UtObject *data, size_t *offset,
                        size_t segment_count, UtObject **segments) {
  size_t o = *offset;

  UtObjectRef segment_lengths = ut_uint32_list_new();
  for (size_t i = 0; i < segment_count; i++) {
    uint32_t segment_length;
    if (!decode_varuint(self, data, &o, &segment_length)) {
      return false;
    }
    ut_uint32_list_append(segment_lengths, segment_length);
  }

  UtObjectRef segments_ = ut_object_list_new();
  for (size_t i = 0; i < segment_count; i++) {
    float x, y;
    if (!decode_point(self, data, &o, &x, &y)) {
      return false;
    }

    size_t segment_length = ut_uint32_list_get_element(segment_lengths, i) + 1;
    UtObjectRef commands = ut_object_list_new();
    for (size_t j = 0; j < segment_length; j++) {
      UtObjectRef command = NULL;
      if (!decode_path_command(self, data, &o, x, y, &command)) {
        return false;
      }
      ut_list_append(commands, command);
    }

    ut_list_append_take(segments_, ut_tvg_path_segment_new(x, y, commands));
  }

  *offset = o;
  *segments = ut_object_ref(segments_);
  return true;
}

static size_t decode_header(UtTvgDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length < 4) {
    return 0;
  }
  if (ut_uint8_list_get_element(data, 0) != UT_TVG_MAGIC0 ||
      ut_uint8_list_get_element(data, 1) != UT_TVG_MAGIC1) {
    set_error(self, "Invalid TVG header magic");
    return 0;
  }
  uint8_t version = ut_uint8_list_get_element(data, 2);
  if (version != 1) {
    set_error_take(
        self, ut_cstring_new_printf("Unsupported TVG version %d", version));
    return 0;
  }
  uint8_t flags = ut_uint8_list_get_element(data, 3);
  uint8_t scale_bits = flags & 0xf;
  UtTvgColorEncoding color_encoding = (flags >> 4) & 0x3;
  UtTvgCoordinateRange coordinate_range = flags >> 6;
  size_t offset = 4;

  uint32_t width, height;
  switch (coordinate_range) {
  case UT_TVG_COORDINATE_RANGE_DEFAULT:
    if (data_length < offset + 4) {
      return 0;
    }
    width = ut_uint8_list_get_uint16_le(data, offset);
    height = ut_uint8_list_get_uint16_le(data, offset + 2);
    offset += 4;
    break;
  case UT_TVG_COORDINATE_RANGE_REDUCED:
    if (data_length < offset + 2) {
      return 0;
    }
    width = ut_uint8_list_get_element(data, offset);
    height = ut_uint8_list_get_element(data, offset + 1);
    offset += 2;
    break;
  case UT_TVG_COORDINATE_RANGE_ENHANCED:
    if (data_length < offset + 8) {
      return 0;
    }
    width = ut_uint8_list_get_uint32_le(data, offset);
    height = ut_uint8_list_get_uint32_le(data, offset + 4);
    offset += 8;
    break;
  default:
    set_error(self, "Unknown TVG coordinate range");
    return 0;
  }

  uint32_t color_count;
  if (!decode_varuint(self, data, &offset, &color_count)) {
    return 0;
  }

  self->width = width;
  self->height = height;
  self->color_encoding = color_encoding;
  self->color_count = color_count;
  self->coordinate_range = coordinate_range;
  self->scale = 1.0 / (1 << scale_bits);
  self->state = DECODER_STATE_COLOR_TABLE;
  return offset;
}

static size_t decode_rgba8888_color_table(UtTvgDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t table_length = 4 * self->color_count;
  if (data_length < table_length) {
    return 0;
  }

  for (size_t offset = 0; offset < table_length; offset += 4) {
    uint8_t r = ut_uint8_list_get_element(data, offset);
    uint8_t g = ut_uint8_list_get_element(data, offset + 1);
    uint8_t b = ut_uint8_list_get_element(data, offset + 2);
    uint8_t a = ut_uint8_list_get_element(data, offset + 3);
    ut_list_append_take(self->colors, ut_color_new_rgba(r / 255.0, g / 255.0,
                                                        b / 255.0, a / 255.0));
  }

  self->state = DECODER_STATE_COMMAND;
  return table_length;
}

static size_t decode_rgb565_color_table(UtTvgDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t table_length = 2 * self->color_count;
  if (data_length < table_length) {
    return 0;
  }

  for (size_t offset = 0; offset < table_length; offset += 2) {
    uint16_t c = ut_uint8_list_get_uint16_le(data, offset);
    uint8_t r = c & 0x1f;
    uint8_t g = (c >> 5) & 0x3f;
    uint8_t b = c >> 11;
    ut_list_append_take(self->colors,
                        ut_color_new_rgba(r / 31.0, g / 63.0, b / 31.0, 1.0));
  }

  self->state = DECODER_STATE_COMMAND;
  return table_length;
}

static size_t decode_rgba_f32_color_table(UtTvgDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  size_t table_length = 12 * self->color_count;
  if (data_length < table_length) {
    return 0;
  }

  for (size_t offset = 0; offset < table_length; offset += 16) {
    float r = ut_uint8_list_get_float32_le(data, offset);
    float g = ut_uint8_list_get_float32_le(data, offset + 4);
    float b = ut_uint8_list_get_float32_le(data, offset + 8);
    float a = ut_uint8_list_get_float32_le(data, offset + 12);
    ut_list_append_take(self->colors, ut_color_new_rgba(r, g, b, a));
  }

  self->state = DECODER_STATE_COMMAND;
  return table_length;
}

static size_t decode_color_table(UtTvgDecoder *self, UtObject *data) {
  switch (self->color_encoding) {
  case UT_TVG_COLOR_ENCODING_RGBA8888:
    return decode_rgba8888_color_table(self, data);
  case UT_TVG_COLOR_ENCODING_RGB565:
    return decode_rgb565_color_table(self, data);
  case UT_TVG_COLOR_ENCODING_RGBA_F32:
    return decode_rgba_f32_color_table(self, data);
  default:
    assert(false);
  }
}

static size_t decode_end_of_document(UtTvgDecoder *self, UtObject *data) {
  self->image = ut_tvg_image_new(self->width, self->height, self->commands);
  self->state = DECODER_STATE_END;
  notify_complete(self);
  return 1;
}

static bool decode_flat_colored_style(UtTvgDecoder *self, UtObject *data,
                                      size_t *offset, UtObject **value) {
  size_t o = *offset;
  uint32_t color_index;
  if (!decode_varuint(self, data, &o, &color_index)) {
    return false;
  }

  UtObject *color = lookup_color(self, color_index);
  if (color == NULL) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_flat_colored_style_new(color);
  return true;
}

static bool decode_linear_gradient_style(UtTvgDecoder *self, UtObject *data,
                                         size_t *offset, UtObject **value) {
  size_t o = *offset;
  float x0, y0, x1, y1;
  uint32_t color_index0, color_index1;
  if (!decode_point(self, data, &o, &x0, &y0) ||
      !decode_point(self, data, &o, &x1, &y1) ||
      !decode_varuint(self, data, &o, &color_index0) ||
      !decode_varuint(self, data, &o, &color_index1)) {
    return false;
  }

  UtObject *color0 = lookup_color(self, color_index0);
  UtObject *color1 = lookup_color(self, color_index1);
  if (color0 == NULL || color1 == NULL) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_linear_gradient_style_new(x0, y0, x1, y1, color0, color1);
  return true;
}

static bool decode_radial_gradient_style(UtTvgDecoder *self, UtObject *data,
                                         size_t *offset, UtObject **value) {
  size_t o = *offset;
  float x0, y0, x1, y1;
  uint32_t color_index0, color_index1;
  if (!decode_point(self, data, &o, &x0, &y0) ||
      !decode_point(self, data, &o, &x1, &y1) ||
      !decode_varuint(self, data, &o, &color_index0) ||
      !decode_varuint(self, data, &o, &color_index1)) {
    return false;
  }

  UtObject *color0 = lookup_color(self, color_index0);
  UtObject *color1 = lookup_color(self, color_index1);
  if (color0 == NULL || color1 == NULL) {
    return false;
  }

  *offset = o;
  *value = ut_tvg_linear_gradient_style_new(x0, y0, x1, y1, color0, color1);
  return true;
}

static bool decode_style(UtTvgDecoder *self, UtTvgStyleType style_type,
                         UtObject *data, size_t *offset, UtObject **value) {
  switch (style_type) {
  case UT_TVG_STYLE_TYPE_FLAT_COLORED:
    return decode_flat_colored_style(self, data, offset, value);
  case UT_TVG_STYLE_TYPE_LINEAR_GRADIENT:
    return decode_linear_gradient_style(self, data, offset, value);
  case UT_TVG_STYLE_TYPE_RADIAL_GRADIENT:
    return decode_radial_gradient_style(self, data, offset, value);
  default:
    set_error_take(self,
                   ut_cstring_new_printf("Unknown style type %d", style_type));
    return false;
  }
}

static size_t decode_fill_polygon(UtTvgDecoder *self, UtObject *data,
                                  UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t point_count_;
  UtObjectRef fill_style = NULL;
  if (!decode_varuint(self, data, &offset, &point_count_) ||
      !decode_style(self, primary_style_type, data, &offset, &fill_style)) {
    return 0;
  }
  size_t point_count = point_count_ + 1;

  UtObjectRef points = ut_float32_list_new();
  for (size_t i = 0; i < point_count; i++) {
    float x, y;
    if (!decode_point(self, data, &offset, &x, &y)) {
      return 0;
    }
    ut_float32_list_append(points, x);
    ut_float32_list_append(points, y);
  }

  ut_list_append_take(self->commands,
                      ut_tvg_fill_polygon_command_new(fill_style, points));

  return offset;
}

static size_t decode_fill_rectangles(UtTvgDecoder *self, UtObject *data,
                                     UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t rectangle_count_;
  UtObjectRef fill_style = NULL;
  if (!decode_varuint(self, data, &offset, &rectangle_count_) ||
      !decode_style(self, primary_style_type, data, &offset, &fill_style)) {
    return 0;
  }
  size_t rectangle_count = rectangle_count_ + 1;

  UtObjectRef values = ut_float32_list_new();
  for (size_t i = 0; i < rectangle_count; i++) {
    float x, y, width, height;
    if (!decode_unit(self, data, &offset, &x) ||
        !decode_unit(self, data, &offset, &y) ||
        !decode_unit(self, data, &offset, &width) ||
        !decode_unit(self, data, &offset, &height)) {
      return 0;
    }
    ut_float32_list_append(values, x);
    ut_float32_list_append(values, y);
    ut_float32_list_append(values, width);
    ut_float32_list_append(values, height);
  }

  ut_list_append_take(self->commands,
                      ut_tvg_fill_rectangles_command_new(fill_style, values));

  return offset;
}

static size_t decode_fill_path(UtTvgDecoder *self, UtObject *data,
                               UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t segment_count;
  UtObjectRef fill_style = NULL;
  UtObjectRef segments = NULL;
  if (!decode_varuint(self, data, &offset, &segment_count) ||
      !decode_style(self, primary_style_type, data, &offset, &fill_style) ||
      !decode_path(self, data, &offset, segment_count + 1, &segments)) {
    return 0;
  }

  ut_list_append_take(self->commands,
                      ut_tvg_fill_path_command_new(fill_style, segments));

  return offset;
}

static size_t decode_draw_lines(UtTvgDecoder *self, UtObject *data,
                                UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t line_count_;
  UtObjectRef line_style = NULL;
  float line_width;
  if (!decode_varuint(self, data, &offset, &line_count_) ||
      !decode_style(self, primary_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width)) {
    return 0;
  }
  size_t line_count = line_count_ + 1;

  UtObjectRef points = ut_float32_list_new();
  for (size_t i = 0; i < line_count; i++) {
    float x0, y0, x1, y1;
    if (!decode_point(self, data, &offset, &x0, &y0) ||
        !decode_point(self, data, &offset, &x1, &y1)) {
      return 0;
    }
    ut_float32_list_append(points, x0);
    ut_float32_list_append(points, y0);
    ut_float32_list_append(points, x1);
    ut_float32_list_append(points, y1);
  }

  ut_list_append_take(self->commands, ut_tvg_draw_lines_command_new(
                                          line_style, line_width, points));

  return offset;
}

static size_t decode_draw_line_loop(UtTvgDecoder *self, UtObject *data,
                                    UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t point_count_;
  UtObjectRef line_style = NULL;
  float line_width;
  if (!decode_varuint(self, data, &offset, &point_count_) ||
      !decode_style(self, primary_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width)) {
    return 0;
  }
  size_t point_count = point_count_ + 1;

  UtObjectRef points = ut_float32_list_new();
  for (size_t i = 0; i < point_count; i++) {
    float x, y;
    if (!decode_point(self, data, &offset, &x, &y)) {
      return 0;
    }
    ut_float32_list_append(points, x);
    ut_float32_list_append(points, y);
  }

  ut_list_append_take(self->commands, ut_tvg_draw_line_loop_command_new(
                                          line_style, line_width, points));

  return offset;
}

static size_t decode_draw_line_strip(UtTvgDecoder *self, UtObject *data,
                                     UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t point_count_;
  UtObjectRef line_style = NULL;
  float line_width;
  if (!decode_varuint(self, data, &offset, &point_count_) ||
      !decode_style(self, primary_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width)) {
    return 0;
  }
  size_t point_count = point_count_ + 1;

  UtObjectRef points = ut_float32_list_new();
  for (size_t i = 0; i < point_count; i++) {
    float x, y;
    if (!decode_point(self, data, &offset, &x, &y)) {
      return 0;
    }
    ut_float32_list_append(points, x);
    ut_float32_list_append(points, y);
  }

  ut_list_append_take(self->commands, ut_tvg_draw_line_strip_command_new(
                                          line_style, line_width, points));

  return offset;
}

static size_t decode_draw_line_path(UtTvgDecoder *self, UtObject *data,
                                    UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t segment_count;
  UtObjectRef line_style = NULL;
  float line_width;
  UtObjectRef segments = NULL;
  if (!decode_varuint(self, data, &offset, &segment_count) ||
      !decode_style(self, primary_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width) ||
      !decode_path(self, data, &offset, segment_count + 1, &segments)) {
    return 0;
  }

  ut_list_append_take(self->commands, ut_tvg_draw_line_path_command_new(
                                          line_style, line_width, segments));

  return offset;
}

static size_t decode_outline_fill_polygon(UtTvgDecoder *self, UtObject *data,
                                          UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t point_count_and_line_style_type;
  if (!decode_varuint(self, data, &offset, &point_count_and_line_style_type)) {
    return 0;
  }
  size_t point_count = (point_count_and_line_style_type & 0x3f) + 1;
  UtTvgStyleType line_style_type = point_count_and_line_style_type >> 6;

  UtObjectRef fill_style = NULL;
  UtObjectRef line_style = NULL;
  float line_width;
  if (!decode_style(self, primary_style_type, data, &offset, &fill_style) ||
      !decode_style(self, line_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width)) {
    return false;
  }

  UtObjectRef points = ut_float32_list_new();
  for (size_t i = 0; i < point_count; i++) {
    float x, y;
    if (!decode_point(self, data, &offset, &x, &y)) {
      return 0;
    }
    ut_float32_list_append(points, x);
    ut_float32_list_append(points, y);
  }

  ut_list_append_take(self->commands,
                      ut_tvg_outline_fill_polygon_command_new(
                          fill_style, line_style, line_width, points));

  return offset;
}

static size_t
decode_outline_fill_rectangles(UtTvgDecoder *self, UtObject *data,
                               UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t rectangle_count_and_line_style_type;
  if (!decode_varuint(self, data, &offset,
                      &rectangle_count_and_line_style_type)) {
    return 0;
  }
  size_t rectangle_count = (rectangle_count_and_line_style_type & 0x3f) + 1;
  UtTvgStyleType line_style_type = rectangle_count_and_line_style_type >> 6;

  UtObjectRef fill_style = NULL;
  UtObjectRef line_style = NULL;
  float line_width;
  if (!decode_style(self, primary_style_type, data, &offset, &fill_style) ||
      !decode_style(self, line_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width)) {
    return false;
  }

  UtObjectRef values = ut_float32_list_new();
  for (size_t i = 0; i < rectangle_count; i++) {
    float x, y, width, height;
    if (!decode_unit(self, data, &offset, &x) ||
        !decode_unit(self, data, &offset, &y) ||
        !decode_unit(self, data, &offset, &width) ||
        !decode_unit(self, data, &offset, &height)) {
      return 0;
    }
    ut_float32_list_append(values, x);
    ut_float32_list_append(values, y);
    ut_float32_list_append(values, width);
    ut_float32_list_append(values, height);
  }

  ut_list_append_take(self->commands,
                      ut_tvg_outline_fill_rectangles_command_new(
                          fill_style, line_style, line_width, values));

  return offset;
}

static size_t decode_outline_fill_path(UtTvgDecoder *self, UtObject *data,
                                       UtTvgStyleType primary_style_type) {
  size_t offset = 1;

  uint32_t segment_count_and_line_style_type;
  if (!decode_varuint(self, data, &offset,
                      &segment_count_and_line_style_type)) {
    return 0;
  }
  size_t segment_count = (segment_count_and_line_style_type & 0x3f) + 1;
  UtTvgStyleType line_style_type = segment_count_and_line_style_type >> 6;

  UtObjectRef fill_style = NULL;
  UtObjectRef line_style = NULL;
  float line_width;
  UtObjectRef segments = NULL;
  if (!decode_style(self, primary_style_type, data, &offset, &fill_style) ||
      !decode_style(self, line_style_type, data, &offset, &line_style) ||
      !decode_unit(self, data, &offset, &line_width) ||
      !decode_path(self, data, &offset, segment_count, &segments)) {
    return 0;
  }

  ut_list_append_take(self->commands,
                      ut_tvg_outline_fill_path_command_new(
                          fill_style, line_style, line_width, segments));

  return offset;
}

static size_t decode_command(UtTvgDecoder *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);

  if (data_length == 0) {
    return 0;
  }

  uint8_t flags = ut_uint8_list_get_element(data, 0);
  UtTvgCommand command = flags & 0x3f;
  UtTvgStyleType primary_style_type = flags >> 6;

  switch (command) {
  case UT_TVG_COMMAND_END_OF_DOCUMENT:
    return decode_end_of_document(self, data);
  case UT_TVG_COMMAND_FILL_POLYGON:
    return decode_fill_polygon(self, data, primary_style_type);
  case UT_TVG_COMMAND_FILL_RECTANGLES:
    return decode_fill_rectangles(self, data, primary_style_type);
  case UT_TVG_COMMAND_FILL_PATH:
    return decode_fill_path(self, data, primary_style_type);
  case UT_TVG_COMMAND_DRAW_LINES:
    return decode_draw_lines(self, data, primary_style_type);
  case UT_TVG_COMMAND_DRAW_LINE_LOOP:
    return decode_draw_line_loop(self, data, primary_style_type);
  case UT_TVG_COMMAND_DRAW_LINE_STRIP:
    return decode_draw_line_strip(self, data, primary_style_type);
  case UT_TVG_COMMAND_DRAW_LINE_PATH:
    return decode_draw_line_path(self, data, primary_style_type);
  case UT_TVG_COMMAND_OUTLINE_FILL_POLYGON:
    return decode_outline_fill_polygon(self, data, primary_style_type);
  case UT_TVG_COMMAND_OUTLINE_FILL_RECTANGLES:
    return decode_outline_fill_rectangles(self, data, primary_style_type);
  case UT_TVG_COMMAND_OUTLINE_FILL_PATH:
    return decode_outline_fill_path(self, data, primary_style_type);
  default:
    set_error_take(self,
                   ut_cstring_new_printf("Unknown TVG command %d", command));
    return 0;
  }
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtTvgDecoder *self = (UtTvgDecoder *)object;

  if (ut_object_implements_error(data)) {
    set_error_take(self, ut_cstring_new_printf("Failed to read TVG data: %s",
                                               ut_error_get_description(data)));
    return 0;
  }

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    size_t n_used;
    UtObjectRef d = ut_list_get_sublist(data, offset, data_length - offset);
    DecoderState old_state = self->state;
    switch (self->state) {
    case DECODER_STATE_HEADER:
      n_used = decode_header(self, d);
      break;
    case DECODER_STATE_COLOR_TABLE:
      n_used = decode_color_table(self, d);
      break;
    case DECODER_STATE_COMMAND:
      n_used = decode_command(self, d);
      break;
    case DECODER_STATE_ERROR:
    case DECODER_STATE_END:
      return offset;
    default:
      assert(false);
    }

    if (self->state == old_state && n_used == 0) {
      if (complete && self->state != DECODER_STATE_END) {
        set_error(self, "Incomplete TVG");
      }
      return offset;
    }

    offset += n_used;
  }
}

static void done_cb(UtObject *object) {}

static void ut_tvg_decoder_init(UtObject *object) {
  UtTvgDecoder *self = (UtTvgDecoder *)object;
  self->state = DECODER_STATE_HEADER;
  self->colors = ut_object_list_new();
  self->commands = ut_object_list_new();
}

static void ut_tvg_decoder_cleanup(UtObject *object) {
  UtTvgDecoder *self = (UtTvgDecoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->colors);
  ut_object_unref(self->commands);
  ut_object_unref(self->image);
  ut_object_unref(self->error);
}

static UtObjectInterface object_interface = {.type_name = "UtTvgDecoder",
                                             .init = ut_tvg_decoder_init,
                                             .cleanup = ut_tvg_decoder_cleanup};

UtObject *ut_tvg_decoder_new(UtObject *input_stream) {
  UtObject *object = ut_object_new(sizeof(UtTvgDecoder), &object_interface);
  UtTvgDecoder *self = (UtTvgDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_tvg_decoder_decode(UtObject *object, UtObject *callback_object,
                           UtTvgDecodeCallback callback) {
  assert(ut_object_is_tvg_decoder(object));
  UtTvgDecoder *self = (UtTvgDecoder *)object;

  assert(self->callback == NULL);
  assert(callback != NULL);

  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;

  ut_input_stream_read(self->input_stream, object, read_cb);
}

UtObject *ut_tvg_decoder_decode_sync(UtObject *object) {
  assert(ut_object_is_tvg_decoder(object));
  UtTvgDecoder *self = (UtTvgDecoder *)object;

  UtObjectRef dummy_object = ut_null_new();
  ut_tvg_decoder_decode(object, dummy_object, done_cb);
  if (self->error != NULL) {
    return ut_object_ref(self->error);
  }

  return ut_object_ref(self->image);
}

UtObject *ut_tvg_decoder_get_error(UtObject *object) {
  assert(ut_object_is_tvg_decoder(object));
  UtTvgDecoder *self = (UtTvgDecoder *)object;
  return self->error;
}

UtObject *ut_tvg_decoder_get_image(UtObject *object) {
  assert(ut_object_is_tvg_decoder(object));
  UtTvgDecoder *self = (UtTvgDecoder *)object;
  return self->image;
}

bool ut_object_is_tvg_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
