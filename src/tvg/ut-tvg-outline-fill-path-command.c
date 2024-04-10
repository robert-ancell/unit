#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *fill_style;
  UtObject *line_style;
  float line_width;
  UtObject *path;
} UtTvgOutlineFillPathCommand;

static char *ut_tvg_outline_fill_path_command_to_string(UtObject *object) {
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;
  ut_cstring_ref fill_style_string = ut_object_to_string(self->fill_style);
  ut_cstring_ref line_style_string = ut_object_to_string(self->line_style);
  ut_cstring_ref path_string = ut_object_to_string(self->path);
  return ut_cstring_new_printf("<UtTvgOutlineFillPathCommand>(%s, %s, %f, %s)",
                               fill_style_string, line_style_string,
                               self->line_width, path_string);
}

static void ut_tvg_outline_fill_path_command_cleanup(UtObject *object) {
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;
  ut_object_unref(self->fill_style);
  ut_object_unref(self->line_style);
  ut_object_unref(self->path);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgOutlineFillPathCommand",
    .to_string = ut_tvg_outline_fill_path_command_to_string,
    .cleanup = ut_tvg_outline_fill_path_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_outline_fill_path_command_new(UtObject *fill_style,
                                               UtObject *line_style,
                                               float line_width,
                                               UtObject *path) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgOutlineFillPathCommand), &object_interface);
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;

  self->fill_style = ut_object_ref(fill_style);
  self->line_style = ut_object_ref(line_style);
  self->line_width = line_width;
  self->path = ut_object_ref(path);

  return object;
}

UtObject *ut_tvg_outline_fill_path_command_get_fill_style(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_path_command(object));
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;
  return self->fill_style;
}

UtObject *ut_tvg_outline_fill_path_command_get_line_style(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_path_command(object));
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;
  return self->line_style;
}

UtObject *ut_tvg_outline_fill_path_command_get_path(UtObject *object) {
  assert(ut_object_is_tvg_outline_fill_path_command(object));
  UtTvgOutlineFillPathCommand *self = (UtTvgOutlineFillPathCommand *)object;
  return self->path;
}

bool ut_object_is_tvg_outline_fill_path_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
