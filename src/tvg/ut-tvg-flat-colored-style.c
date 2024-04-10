#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *color;
} UtTvgFlatColoredStyle;

static char *ut_tvg_flat_colored_style_to_string(UtObject *object) {
  UtTvgFlatColoredStyle *self = (UtTvgFlatColoredStyle *)object;
  ut_cstring_ref color_string = ut_object_to_string(self->color);
  return ut_cstring_new_printf("<UtTvgFlatColoredStyle>(%s)", color_string);
}

static void ut_tvg_flat_colored_style_cleanup(UtObject *object) {
  UtTvgFlatColoredStyle *self = (UtTvgFlatColoredStyle *)object;
  ut_object_unref(self->color);
}

static UtTvgStyleInterface tvg_style_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgFlatColoredStyle",
    .to_string = ut_tvg_flat_colored_style_to_string,
    .cleanup = ut_tvg_flat_colored_style_cleanup,
    .interfaces = {{&ut_tvg_style_id, &tvg_style_interface}, {NULL, NULL}}};

UtObject *ut_tvg_flat_colored_style_new(UtObject *color) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgFlatColoredStyle), &object_interface);
  UtTvgFlatColoredStyle *self = (UtTvgFlatColoredStyle *)object;

  self->color = ut_object_ref(color);

  return object;
}

UtObject *ut_tvg_flat_colored_style_get_color(UtObject *object) {
  assert(ut_object_is_tvg_flat_colored_style(object));
  UtTvgFlatColoredStyle *self = (UtTvgFlatColoredStyle *)object;
  return self->color;
}

bool ut_object_is_tvg_flat_colored_style(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
