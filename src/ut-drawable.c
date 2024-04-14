#include <assert.h>

#include "ut.h"

int ut_drawable_id = 0;

// Returns true if the two lines intersect.
static bool lines_intersect(float x00, float y00, float x01, float y01,
                            float x10, float y10, float x11, float y11) {
  // Convert lines to form [x, y] = s[mx, my] + [x0, y0]
  float mx0 = x01 - x00;
  float my0 = y01 - y00;
  float mx1 = x11 - x10;
  float my1 = y11 - y10;

  // For both lines to be equal:
  // s[mx0, my0] + [x00, y00] = t[mx1, my1] + [x10, y10]
  //
  // s[mx0, my0] - t[mx1, my1] = [x10 - x00, y10 - y00]
  //
  // Matrix form:
  // [mx0, -mx1][s] = [x10 - x00]
  // [my0, -my1][t]   [y10 - y00]
  //
  // Invert matrix to solve:
  // [s] = [mx0, -mx1]-1[x10 - x00]
  // [t]   [my0, -my1]  [y10 - y00]

  // Calculate divisor for inverted 2x2 matrix:
  // [a b]-1 = 1/(ad - bc) * [ d, -b]
  // [c d]                   [-c,  a]
  float d = (mx0 * -my1) - (-mx1 * my0);
  if (d == 0) {
    // Line are parallel - no intersection.
    return false;
  }

  float dx = x10 - x00;
  float dy = y10 - y00;

  // Intersection point on first line:
  float s = -my1 * dx + mx1 * dy;
  if (s <= 0 || s >= 1) {
    // Outside of line
    return false;
  }

  // Intersection point on second line:
  float t = -my0 * dx + mx0 * dy;
  if (t <= 0 || t >= 1) {
    // Outside of line
    return false;
  }

  return true;
}

// Gets the [i]th vertex from [verticies].
static void get_vertex(UtObject *verticies, size_t i, float *x, float *y) {
  *x = ut_float32_list_get_element(verticies, i * 2);
  *y = ut_float32_list_get_element(verticies, i * 2 + 1);
}

// Get the vertex index that a node is using.
static size_t get_vertex_index(UtObject *nodes, size_t i) {
  return ut_uint16_list_get_element(nodes, i);
}

// Remove the first convex node from the polygon made up of [nodes].
// Adds the clipped triangle to [triangles].
static void clip_node(UtObject *verticies, UtObject *nodes,
                      UtObject *triangles) {
  size_t nodes_length = ut_list_get_length(nodes);

  // Find the first convex node.
  for (size_t i = 0; i < nodes_length; i++) {
    size_t prev_i = (i + nodes_length - 1) % nodes_length;
    size_t next_i = (i + 1) % nodes_length;

    size_t curr_node = get_vertex_index(nodes, i);
    size_t prev_node = get_vertex_index(nodes, prev_i);
    size_t next_node = get_vertex_index(nodes, next_i);

    float x0, y0, x1, y1;
    get_vertex(verticies, prev_node, &x0, &y0);
    get_vertex(verticies, next_node, &x1, &y1);

    // Check if the edge formed by cutting off this node intersects with any of
    // the polygon edges.
    for (size_t j = 0; j < nodes_length; j++) {
      if (j == prev_i || j == i) {
        continue;
      }

      float ex0, ey0, ex1, ey1;
      get_vertex(verticies, get_vertex_index(nodes, j), &ex0, &ey0);
      get_vertex(verticies, get_vertex_index(nodes, (j + 1) % nodes_length),
                 &ex1, &ey1);
      if (!lines_intersect(x0, y0, x1, y1, ex0, ey0, ex1, ey1)) {
        ut_list_remove(nodes, i, 1);
        ut_uint16_list_append(triangles, prev_node);
        ut_uint16_list_append(triangles, curr_node);
        ut_uint16_list_append(triangles, next_node);
        return;
      }
    }
  }

  printf("%s\n", ut_object_to_string(nodes));
  assert(false);
}

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
  ut_drawable_render_mesh(object, verticies, triangles, color);
}

void ut_drawable_render_polygon(UtObject *object, UtObject *verticies,
                                UtObject *color) {
  UtObjectRef triangles = ut_uint16_list_new();

  // Convert to mesh by clipping off convex nodes until a single triangle is
  // left.
  size_t verticies_length = ut_list_get_length(verticies);
  size_t nodes_length = verticies_length / 2;
  if (nodes_length < 3) {
    // Not a polygon...
    return;
  }
  assert(nodes_length <= 65535);
  UtObjectRef nodes = ut_uint16_list_new();
  for (size_t i = 0; i < nodes_length; i++) {
    ut_uint16_list_append(nodes, i);
  }
  while (nodes_length > 3) {
    clip_node(verticies, nodes, triangles);
    nodes_length--;
  }
  ut_uint16_list_append(triangles, get_vertex_index(nodes, 0));
  ut_uint16_list_append(triangles, get_vertex_index(nodes, 1));
  ut_uint16_list_append(triangles, get_vertex_index(nodes, 2));

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
