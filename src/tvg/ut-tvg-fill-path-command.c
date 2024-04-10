#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  UtObject *segments;
} UtTvgFillPathCommand;

static char *ut_tvg_fill_path_command_to_string(UtObject *object) {
  UtTvgFillPathCommand *self = (UtTvgFillPathCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref path_string = ut_object_to_string(self->segments);
  return ut_cstring_new_printf("<UtTvgFillPathCommand>(%s, %s)", style_string,
                               path_string);
}

static void ut_tvg_fill_path_command_cleanup(UtObject *object) {
  UtTvgFillPathCommand *self = (UtTvgFillPathCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->segments);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgFillPathCommand",
    .to_string = ut_tvg_fill_path_command_to_string,
    .cleanup = ut_tvg_fill_path_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_fill_path_command_new(UtObject *style, UtObject *segments) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgFillPathCommand), &object_interface);
  UtTvgFillPathCommand *self = (UtTvgFillPathCommand *)object;

  self->style = ut_object_ref(style);
  self->segments = ut_object_ref(segments);

  return object;
}

UtObject *ut_tvg_fill_path_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_fill_path_command(object));
  UtTvgFillPathCommand *self = (UtTvgFillPathCommand *)object;
  return self->style;
}

UtObject *ut_tvg_fill_path_command_get_segments(UtObject *object) {
  assert(ut_object_is_tvg_fill_path_command(object));
  UtTvgFillPathCommand *self = (UtTvgFillPathCommand *)object;
  return self->segments;
}

bool ut_object_is_tvg_fill_path_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
