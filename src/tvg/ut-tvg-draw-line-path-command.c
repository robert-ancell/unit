#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  float line_width;
  UtObject *segments;
} UtTvgDrawLinePathCommand;

static char *ut_tvg_draw_line_path_command_to_string(UtObject *object) {
  UtTvgDrawLinePathCommand *self = (UtTvgDrawLinePathCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref path_string = ut_object_to_string(self->segments);
  return ut_cstring_new_printf("<UtTvgDrawLinePathCommand>(%s, %f, %s)",
                               style_string, self->line_width, path_string);
}

static void ut_tvg_draw_line_path_command_cleanup(UtObject *object) {
  UtTvgDrawLinePathCommand *self = (UtTvgDrawLinePathCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->segments);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgDrawLinePathCommand",
    .to_string = ut_tvg_draw_line_path_command_to_string,
    .cleanup = ut_tvg_draw_line_path_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_draw_line_path_command_new(UtObject *style, float line_width,
                                            UtObject *segments) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgDrawLinePathCommand), &object_interface);
  UtTvgDrawLinePathCommand *self = (UtTvgDrawLinePathCommand *)object;

  self->style = ut_object_ref(style);
  self->line_width = line_width;
  self->segments = ut_object_ref(segments);

  return object;
}

UtObject *ut_tvg_draw_line_path_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_path_command(object));
  UtTvgDrawLinePathCommand *self = (UtTvgDrawLinePathCommand *)object;
  return self->style;
}

UtObject *ut_tvg_draw_line_path_command_get_segments(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_path_command(object));
  UtTvgDrawLinePathCommand *self = (UtTvgDrawLinePathCommand *)object;
  return self->segments;
}

bool ut_object_is_tvg_draw_line_path_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
