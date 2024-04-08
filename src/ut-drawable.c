#include <assert.h>
#include <math.h>

#include "ut.h"

int ut_drawable_id = 0;

static void add_quad(UtObject *verticies, UtObject *triangles, double x0,
                     double y0, double x1, double y1, double x2, double y2,
                     double x3, double y3) {
  size_t verticies_length = ut_list_get_length(verticies);

  ut_float64_list_append(verticies, x0);
  ut_float64_list_append(verticies, y0);
  ut_float64_list_append(verticies, x1);
  ut_float64_list_append(verticies, y1);
  ut_float64_list_append(verticies, x2);
  ut_float64_list_append(verticies, y2);
  ut_float64_list_append(verticies, x3);
  ut_float64_list_append(verticies, y3);

  ut_uint16_list_append(triangles, verticies_length);
  ut_uint16_list_append(triangles, verticies_length + 1);
  ut_uint16_list_append(triangles, verticies_length + 2);
  ut_uint16_list_append(triangles, verticies_length);
  ut_uint16_list_append(triangles, verticies_length + 2);
  ut_uint16_list_append(triangles, verticies_length + 3);
}

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

void ut_drawable_render_line(UtObject *object, UtObject *verticies,
                             double width, UtObject *color) {
  UtObjectRef mesh_verticies = ut_float64_list_new();
  UtObjectRef mesh_triangles = ut_uint16_list_new();

  size_t verticies_length = ut_list_get_length(verticies);
  if (verticies_length < 4) {
    return;
  }

  double x0 = ut_float64_list_get_element(verticies, 0);
  double y0 = ut_float64_list_get_element(verticies, 1);
  for (size_t i = 2; i < verticies_length; i += 2) {
    double x1 = ut_float64_list_get_element(verticies, i);
    double y1 = ut_float64_list_get_element(verticies, i + 1);

    double dx = y1 - y0;
    double dy = x0 - x1;
    if (dx != 0 || dy != 0) {
      double dd = sqrt(dx * dx + dy * dy);
      dx *= 0.5 * width / dd;
      dy *= 0.5 * width / dd;
       printf("%f %f\n", dx, dy);
      // FIXME actually intersect points
      add_quad(mesh_verticies, mesh_triangles, x0 + dx, y0 + dy, x0 - dx,
               y0 - dy, x1 - dx, y1 - dy, x1 + dx, y1 + dy);
    }

    x0 = x1;
    y0 = y1;
  }

   printf("%s %f\n", ut_object_to_string(verticies), width);
   printf("%s\n", ut_object_to_string(mesh_verticies));
   printf("%s\n", ut_object_to_string(mesh_triangles));   

  ut_drawable_render_mesh(object, mesh_verticies, mesh_triangles, color);
}

bool ut_object_implements_drawable(UtObject *object) {
  return ut_object_get_interface(object, &ut_drawable_id) != NULL;
}
