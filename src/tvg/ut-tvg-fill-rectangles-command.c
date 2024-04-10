#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *style;
  UtObject *values;
} UtTvgFillRectanglesCommand;

static char *ut_tvg_fill_rectangles_command_to_string(UtObject *object) {
  UtTvgFillRectanglesCommand *self = (UtTvgFillRectanglesCommand *)object;
  ut_cstring_ref style_string = ut_object_to_string(self->style);
  ut_cstring_ref values_string = ut_object_to_string(self->values);
  return ut_cstring_new_printf("<UtTvgFillRectanglesCommand>(%s, %s)",
                               style_string, values_string);
}

static void ut_tvg_fill_rectangles_command_cleanup(UtObject *object) {
  UtTvgFillRectanglesCommand *self = (UtTvgFillRectanglesCommand *)object;
  ut_object_unref(self->style);
  ut_object_unref(self->values);
}

static UtTvgCommandInterface tvg_command_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgFillRectanglesCommand",
    .to_string = ut_tvg_fill_rectangles_command_to_string,
    .cleanup = ut_tvg_fill_rectangles_command_cleanup,
    .interfaces = {{&ut_tvg_command_id, &tvg_command_interface}, {NULL, NULL}}};

UtObject *ut_tvg_fill_rectangles_command_new(UtObject *style,
                                             UtObject *values) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgFillRectanglesCommand), &object_interface);
  UtTvgFillRectanglesCommand *self = (UtTvgFillRectanglesCommand *)object;

  self->style = ut_object_ref(style);
  self->values = ut_object_ref(values);

  return object;
}

UtObject *ut_tvg_fill_rectangles_command_get_style(UtObject *object) {
  assert(ut_object_is_tvg_fill_rectangles_command(object));
  UtTvgFillRectanglesCommand *self = (UtTvgFillRectanglesCommand *)object;
  return self->style;
}

UtObject *ut_tvg_fill_rectangles_command_get_values(UtObject *object) {
  assert(ut_object_is_tvg_fill_rectangles_command(object));
  UtTvgFillRectanglesCommand *self = (UtTvgFillRectanglesCommand *)object;
  return self->values;
}

bool ut_object_is_tvg_fill_rectangles_command(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
