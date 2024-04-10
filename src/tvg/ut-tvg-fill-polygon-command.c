#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  UtObject *points;
} UtTvgFillPolygonCommand;

static char *ut_tvg_fill_polygon_command_to_string(UtObject *object) {
  UtTvgFillPolygonCommand *self = (UtTvgFillPolygonCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref points_string = ut_object_to_string(self->points);
  return ut_cstring_new_printf("<UtTvgFillPolygonCommand>(%s, %s)",
                               style_string, points_string);
}

static void ut_tvg_fill_polygon_command_cleanup(UtObject *object) {
  UtTvgFillPolygonCommand *self = (UtTvgFillPolygonCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->points);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgFillPolygonCommand",
    .to_string = ut_tvg_fill_polygon_command_to_string,
    .cleanup = ut_tvg_fill_polygon_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_fill_polygon_command_new(UtObject *style, UtObject *points) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgFillPolygonCommand), &object_interface);
  UtTvgFillPolygonCommand *self = (UtTvgFillPolygonCommand *)object;

  self->style = ut_object_ref(style);
  self->points = ut_object_ref(points);

  return object;
}

UtObject *ut_tvg_fill_polygon_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_fill_polygon_command(object));
  UtTvgFillPolygonCommand *self = (UtTvgFillPolygonCommand *)object;
  return self->style;
}

UtObject *ut_tvg_fill_polygon_command_get_points(UtObject *object) {
  assert(ut_object_is_tvg_fill_polygon_command(object));
  UtTvgFillPolygonCommand *self = (UtTvgFillPolygonCommand *)object;
  return self->points;
}

bool ut_object_is_tvg_fill_polygon_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
