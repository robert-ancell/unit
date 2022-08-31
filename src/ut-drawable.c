#include <assert.h>

#include "ut.h"

int ut_drawable_id = 0;

void ut_drawable_clear(UtObject *object, UtObject *color) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->clear(object, color);
}

void ut_drawable_render_box(UtObject *object, double x, double y, double width,
                            double height, UtObject *color) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->render_box(object, x, y, width, height, color);
}

bool ut_object_implements_drawable(UtObject *object) {
  return ut_object_get_interface(object, &ut_drawable_id) != NULL;
}
