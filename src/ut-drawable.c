#include <assert.h>

#include "ut.h"

int ut_drawable_id = 0;

void ut_drawable_clear(UtObject *object, UtObject *color) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->clear(object, color);
}

void ut_drawable_render_box(UtObject *object, float x, float y, float width,
                            float height, UtObject *color) {

  UtObjectRef verticies = ut_float32_list_new_from_elements(
      8, x, y, x + width, y, x + width, y + height, x, y + height);
  UtObjectRef triangles = ut_uint16_list_new_from_elements(6, 0, 1, 2, 0, 2, 3);
  UtObjectRef mesh = ut_mesh_new(verticies, triangles);
  ut_drawable_render_mesh(object, mesh, color);
}

void ut_drawable_render_mesh(UtObject *object, UtObject *mesh,
                             UtObject *color) {
  UtDrawableInterface *drawable_interface =
      ut_object_get_interface(object, &ut_drawable_id);
  assert(drawable_interface != NULL);
  drawable_interface->render_mesh(object, mesh, color);
}

bool ut_object_implements_drawable(UtObject *object) {
  return ut_object_get_interface(object, &ut_drawable_id) != NULL;
}
