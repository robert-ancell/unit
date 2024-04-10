#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  float x0;
  float y0;
  float x1;
  float y1;
  UtObject *color0;
  UtObject *color1;
} UtTvgRadialGradientStyle;

static char *ut_tvg_radial_gradient_style_to_string(UtObject *object) {
  UtTvgRadialGradientStyle *self = (UtTvgRadialGradientStyle *)object;
  ut_cstring_ref color0_string = ut_object_to_string(self->color0);
  ut_cstring_ref color1_string = ut_object_to_string(self->color1);
  return ut_cstring_new_printf(
      "<UtTvgRadialGradientStyle>(%f, %f, %f, %f, %s, %s)", self->x0, self->y0,
      self->x1, self->y1, color0_string, color1_string);
}

static void ut_tvg_radial_gradient_style_cleanup(UtObject *object) {
  UtTvgRadialGradientStyle *self = (UtTvgRadialGradientStyle *)object;
  ut_object_unref(self->color0);
  ut_object_unref(self->color1);
}

static UtTvgStyleInterface tvg_style_interface = {};

static UtObjectInterface object_interface = {
    .type_name = "UtTvgRadialGradientStyle",
    .to_string = ut_tvg_radial_gradient_style_to_string,
    .cleanup = ut_tvg_radial_gradient_style_cleanup,
    .interfaces = {{&ut_tvg_style_id, &tvg_style_interface}, {NULL, NULL}}};

UtObject *ut_tvg_radial_gradient_style_new(float x0, float y0, float x1,
                                           float y1, UtObject *color0,
                                           UtObject *color1) {
  UtObject *object =
      ut_object_new(sizeof(UtTvgRadialGradientStyle), &object_interface);
  UtTvgRadialGradientStyle *self = (UtTvgRadialGradientStyle *)object;

  self->x0 = x0;
  self->y0 = y0;
  self->x1 = x1;
  self->y1 = y1;
  self->color0 = ut_object_ref(color0);
  self->color1 = ut_object_ref(color1);

  return object;
}

UtObject *ut_tvg_radial_gradient_style_get_color0(UtObject *object) {
  assert(ut_object_is_tvg_radial_gradient_style(object));
  UtTvgRadialGradientStyle *self = (UtTvgRadialGradientStyle *)object;
  return self->color0;
}

UtObject *ut_tvg_radial_gradient_style_get_color1(UtObject *object) {
  assert(ut_object_is_tvg_radial_gradient_style(object));
  UtTvgRadialGradientStyle *self = (UtTvgRadialGradientStyle *)object;
  return self->color1;
}

bool ut_object_is_tvg_radial_gradient_style(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
