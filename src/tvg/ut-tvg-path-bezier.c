#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float line_width;
  float control_x0;
  float control_y0;
  float control_x1;
  float control_y1;
  float x;
  float y;
} UtTvgPathBezier;

static char *ut_tvg_path_bezier_to_string(UtObject *object) {
  UtTvgPathBezier *self = (UtTvgPathBezier *)object;
  return ut_cstring_new_printf("<UtTvgPathBezier>(%f, %f, %f, %f, %f, %f, %f)",
                               self->line_width, self->control_x0,
                               self->control_y0, self->control_x1,
                               self->control_y1, self->x, self->y);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTvgPathBezier", .to_string = ut_tvg_path_bezier_to_string};

UtObject *ut_tvg_path_bezier_new(float line_width, float control_x0,
                                 float control_y0, float control_x1,
                                 float control_y1, float x, float y) {
  UtObject *object = ut_object_new(sizeof(UtTvgPathBezier), &object_interface);
  UtTvgPathBezier *self = (UtTvgPathBezier *)object;

  self->line_width = line_width;
  self->control_x0 = control_x0;
  self->control_y0 = control_y0;
  self->control_x1 = control_x1;
  self->control_y1 = control_y1;
  self->x = x;
  self->y = y;

  return object;
}

UtObject *ut_tvg_path_bezier_new_quadratic(float line_width, float control_x,
                                           float control_y, float x, float y) {
  return ut_tvg_path_bezier_new(line_width, control_x, control_y, control_x,
                                control_y, x, y);
}

bool ut_object_is_tvg_path_bezier(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
