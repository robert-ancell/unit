#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float x;
  float y;
  UtObject *commands;
} UtTvgPathSegment;

static char *ut_tvg_path_segment_to_string(UtObject *object) {
  UtTvgPathSegment *self = (UtTvgPathSegment *)object;
  ut_cstring_ref commands_string = ut_object_to_string(self->commands);
  return ut_cstring_new_printf("<UtTvgPathSegment>(%f, %f, %s)", self->x,
                               self->y, commands_string);
}

static void ut_tvg_path_segment_cleanup(UtObject *object) {
  UtTvgPathSegment *self = (UtTvgPathSegment *)object;
  ut_object_unref(self->commands);
}

static UtObjectInterface object_interface = {
    .type_name = "UtTvgPathSegment",
    .to_string = ut_tvg_path_segment_to_string,
    .cleanup = ut_tvg_path_segment_cleanup};

UtObject *ut_tvg_path_segment_new(float x, float y, UtObject *commands) {
  UtObject *object = ut_object_new(sizeof(UtTvgPathSegment), &object_interface);
  UtTvgPathSegment *self = (UtTvgPathSegment *)object;

  self->x = x;
  self->y = y;
  self->commands = ut_object_ref(commands);

  return object;
}

UtObject *ut_tvg_path_segment_get_commands(UtObject *object) {
  assert(ut_object_is_tvg_path_segment(object));
  UtTvgPathSegment *self = (UtTvgPathSegment *)object;
  return self->commands;
}

bool ut_object_is_tvg_path_segment(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
