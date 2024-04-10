#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *fill_style;
  UtObject *line_style;
  float line_width;
  UtObject *values;
} UtTvgOutlineFillRectanglesCommand;

static char *
ut_tvg_outline_fill_rectangles_command_to_string(UtObject *object) {
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;
  ut_cstring_ref fill_style_string = ut_object_to_string(self->fill_style);
  ut_cstring_ref line_style_string = ut_object_to_string(self->line_style);
  ut_cstring_ref values_string = ut_object_to_string(self->values);
  return ut_cstring_new_printf(
      "<UtTvgOutlineFillRectanglesCommand>(%s, %s, %f, %s)", fill_style_string,
      line_style_string, self->line_width, values_string);
}

static void ut_tvg_outline_fill_rectangles_command_cleanup(UtObject *object) {
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;
  ut_object_unref(self->fill_style);
  ut_object_unref(self->line_style);
  ut_object_unref(self->values);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgOutlineFillRectanglesCommand",
    .to_string = ut_tvg_outline_fill_rectangles_command_to_string,
    .cleanup = ut_tvg_outline_fill_rectangles_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_outline_fill_rectangles_command_new(UtObject *fill_style,
                                                     UtObject *line_style,
                                                     float line_width,
                                                     UtObject *values) {
  UtObject *object = ut_object_new(sizeof(UtTvgOutlineFillRectanglesCommand),
                                   &object_interface);
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;

  self->fill_style = ut_object_ref(fill_style);
  self->line_style = ut_object_ref(line_style);
  self->line_width = line_width;
  self->values = ut_object_ref(values);

  return object;
}

UtObject *
ut_tvg_outline_fill_rectangles_command_get_fill_style(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_rectangles_command(object));
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;
  return self->fill_style;
}

UtObject *
ut_tvg_outline_fill_rectangles_command_get_line_style(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_rectangles_command(object));
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;
  return self->line_style;
}

UtObject *ut_tvg_outline_fill_rectangles_command_get_values(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_rectangles_command(object));
  UtTvgOutlineFillRectanglesCommand *self =
      (UtTvgOutlineFillRectanglesCommand *)object;
  return self->values;
}

bool ut_object_is_tvg_outline_fill_rectangles_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
