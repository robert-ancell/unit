#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float line_width;
  float x;
  float y;
} UtTvgPathLine;

static char *ut_tvg_path_line_to_string(UtObject *object) {
  UtTvgPathLine *self = (UtTvgPathLine *)object;
  return ut_cstring_new_printf("<UtTvgPathLine>(%f, %f, %f)", self->line_width,
                               self->x, self->y);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTvgPathLine", .to_string = ut_tvg_path_line_to_string};

UtObject *ut_tvg_path_line_new(float line_width, float x, float y) {
  UtObject *object = ut_object_new(sizeof(UtTvgPathLine), &object_interface);
  UtTvgPathLine *self = (UtTvgPathLine *)object;

  self->line_width = line_width;
  self->x = x;
  self->y = y;

  return object;
}

bool ut_object_is_tvg_path_line(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
