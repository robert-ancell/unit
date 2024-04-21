#include <assert.h>
#include <stdio.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *verticies;
  UtObject *triangles;
} UtMesh;

static void ut_mesh_cleanup(UtObject *object) {
  UtMesh *self = (UtMesh *)object;
  ut_object_unref(self->verticies);
  ut_object_unref(self->triangles);
}

static char *ut_mesh_to_string(UtObject *object) {
  UtMesh *self = (UtMesh *)object;
  ut_cstring_ref verticies_string = ut_object_to_string(self->verticies);
  ut_cstring_ref triangles_string = ut_object_to_string(self->triangles);
  return ut_cstring_new_printf("<UtMesh>(%s, %s)", verticies_string,
                               triangles_string);
}

static UtObjectInterface object_interface = {.type_name = "UtMesh",
                                             .cleanup = ut_mesh_cleanup,
                                             .to_string = ut_mesh_to_string};

UtObject *ut_mesh_new(UtObject *verticies, UtObject *triangles) {
  UtObject *object = ut_object_new(sizeof(UtMesh), &object_interface);
  UtMesh *self = (UtMesh *)object;

  size_t verticies_length = ut_list_get_length(verticies);
  size_t triangles_length = ut_list_get_length(triangles);
  assert(triangles_length % 3 == 0);
  for (size_t i = 0; i < triangles_length; i++) {
    uint16_t v = ut_uint16_list_get_element(triangles, i);
    assert(v < verticies_length);
  }

  self->verticies = ut_object_ref(verticies);
  self->triangles = ut_object_ref(triangles);

  return object;
}

UtObject *ut_mesh_get_verticies(UtObject *object) {
  assert(ut_object_is_mesh(object));
  UtMesh *self = (UtMesh *)object;
  return self->verticies;
}

UtObject *ut_mesh_get_triangles(UtObject *object) {
  assert(ut_object_is_mesh(object));
  UtMesh *self = (UtMesh *)object;
  return self->triangles;
}

size_t ut_mesh_get_n_triangles(UtObject *object) {
  assert(ut_object_is_mesh(object));
  UtMesh *self = (UtMesh *)object;
  return ut_list_get_length(self->triangles) / 3;
}

void ut_mesh_get_triangle(UtObject *object, size_t index, float *x0, float *y0,
                          float *x1, float *y1, float *x2, float *y2) {
  assert(ut_object_is_mesh(object));
  UtMesh *self = (UtMesh *)object;

  size_t triangles_length = ut_list_get_length(self->triangles);
  assert(index < triangles_length / 3);

  uint16_t v0 = ut_uint16_list_get_element(self->triangles, index * 3);
  uint16_t v1 = ut_uint16_list_get_element(self->triangles, index * 3 + 1);
  uint16_t v2 = ut_uint16_list_get_element(self->triangles, index * 3 + 2);
  *x0 = ut_float32_list_get_element(self->verticies, v0 * 2);
  *x0 = ut_float32_list_get_element(self->verticies, v0 * 2 + 1);
  *x1 = ut_float32_list_get_element(self->verticies, v1 * 2);
  *x1 = ut_float32_list_get_element(self->verticies, v1 * 2 + 1);
  *x2 = ut_float32_list_get_element(self->verticies, v2 * 2);
  *x2 = ut_float32_list_get_element(self->verticies, v2 * 2 + 1);
}

bool ut_object_is_mesh(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
