#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  float line_width;
  UtObject *points;
} UtTvgDrawLineLoopCommand;

static char *ut_tvg_draw_line_loop_command_to_string(UtObject *object) {
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref points_string = ut_object_to_string(self->points);
  return ut_cstring_new_printf("<UtTvgDrawLineLoopCommand>(%s, %f, %s)",
                               style_string, self->line_width, points_string);
}

static void ut_tvg_draw_line_loop_command_cleanup(UtObject *object) {
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->points);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgDrawLineLoopCommand",
    .to_string = ut_tvg_draw_line_loop_command_to_string,
    .cleanup = ut_tvg_draw_line_loop_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_draw_line_loop_command_new(UtObject *style, float line_width,
                                            UtObject *points) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgDrawLineLoopCommand), &object_interface);
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;

  self->style = ut_object_ref(style);
  self->line_width = line_width;
  self->points = ut_object_ref(points);

  return object;
}

UtObject *ut_tvg_draw_line_loop_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_loop_command(object));
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;
  return self->style;
}

float ut_tvg_draw_line_loop_command_get_line_width(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_loop_command(object));
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;
  return self->line_width;
}

UtObject *ut_tvg_draw_line_loop_command_get_points(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_loop_command(object));
  UtTvgDrawLineLoopCommand *self = (UtTvgDrawLineLoopCommand *)object;
  return self->points;
}

bool ut_object_is_tvg_draw_line_loop_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
