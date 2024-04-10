#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  float line_width;
  UtObject *points;
} UtTvgDrawLineStripCommand;

static char *ut_tvg_draw_line_strip_command_to_string(UtObject *object) {
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref points_string = ut_object_to_string(self->points);
  return ut_cstring_new_printf("<UtTvgDrawLineStripCommand>(%s, %f, %s)",
                               style_string, self->line_width, points_string);
}

static void ut_tvg_draw_line_strip_command_cleanup(UtObject *object) {
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->points);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgDrawLineStripCommand",
    .to_string = ut_tvg_draw_line_strip_command_to_string,
    .cleanup = ut_tvg_draw_line_strip_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_draw_line_strip_command_new(UtObject *style, float line_width,
                                             UtObject *points) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgDrawLineStripCommand), &object_interface);
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;

  self->style = ut_object_ref(style);
  self->line_width = line_width;
  self->points = ut_object_ref(points);

  return object;
}

UtObject *ut_tvg_draw_line_strip_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_strip_command(object));
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;
  return self->style;
}

float ut_tvg_draw_line_strip_command_get_line_width(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_strip_command(object));
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;
  return self->line_width;
}

UtObject *ut_tvg_draw_line_strip_command_get_points(UtObject *object) {
  assert(ut_object_is_tvg_draw_line_strip_command(object));
  UtTvgDrawLineStripCommand *self = (UtTvgDrawLineStripCommand *)object;
  return self->points;
}

bool ut_object_is_tvg_draw_line_strip_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
