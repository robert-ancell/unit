#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float line_width;
  float radius_x;
  float radius_y;
  float rotation;
  float x;
  float y;
} UtTvgPathArc;

static char *ut_tvg_path_arc_to_string(UtObject *object) {
  UtTvgPathArc *self = (UtTvgPathArc *)object;
  return ut_cstring_new_printf("<UtTvgPathArc>(%f, %f, %f, %f, %f, %f)",
                               self->line_width, self->radius_x, self->radius_y,
                               self->rotation, self->x, self->y);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTvgPathArc", .to_string = ut_tvg_path_arc_to_string};

UtObject *ut_tvg_path_arc_new(float line_width, float radius_x, float radius_y,
                              float rotation, float x, float y) {
  UtObject *object = ut_object_new(sizeof(UtTvgPathArc), &object_interface);
  UtTvgPathArc *self = (UtTvgPathArc *)object;

  self->line_width = line_width;
  self->radius_x = radius_x;
  self->radius_y = radius_y;
  self->rotation = rotation;
  self->x = x;
  self->y = y;

  return object;
}

UtObject *ut_tvg_path_arc_new_circle(float line_width, float radius, float x,
                                     float y) {
  return ut_tvg_path_arc_new(line_width, radius, radius, 0, x, y);
}

bool ut_object_is_tvg_path_arc(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
