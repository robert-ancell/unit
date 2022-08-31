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

  UtObjectRef verticies = ut_float64_list_new_from_elements(
      8, x, y, x + width, y, x + width, y + height, x, y + height);
  UtObjectRef triangles = ut_uint16_list_new_from_elements(6, 0, 1, 2, 0, 2, 3);
  ut_drawable_render_mesh(object, verticies, triangles, color);
}

void ut_drawable_render_mesh(UtObject *object, UtObject *vertices,
                             UtObject *triangles, UtObject *color) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->render_mesh(object, vertices, triangles, color);
}

bool ut_object_implements_drawable(UtObject *object) {
  return ut_object_get_interface(object, &ut_drawable_id) != NULL;
}
