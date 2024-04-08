#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*clear)(UtObject *object, UtObject *color);
  void (*render_mesh)(UtObject *object, UtObject *verticies,
                      UtObject *triangles, UtObject *color);
} UtDrawableInterface;

extern int ut_drawable_id;

/// Clear the drawable to [color].
///
/// !arg-type color UtColor
void ut_drawable_clear(UtObject *object, UtObject *color);

/// Draw a filled box of size [width]x[height] at [x],[y] filled with [color].
///
/// !arg-type color UtColor
void ut_drawable_render_box(UtObject *object, double x, double y, double width,
                            double height, UtObject *color);

/// Draws [triangles] using [verticies] filled with [color].
///
/// !arg-type verticies UtFloat64List
/// !arg-type triangles UtUint16List
/// !arg-type color UtColor
void ut_drawable_render_mesh(UtObject *object, UtObject *verticies,
                             UtObject *triangles, UtObject *color);

/// Draws a line through [verticies] with [width] pixels and filled with
/// [color].
///
/// !arg-type verticies UtFloat64List
/// !arg-type color UtColor
void ut_drawable_render_line(UtObject *object, UtObject *verticies,
                             double width, UtObject *color);

/// Returns [true] if [object] is a [UtDrawable].
bool ut_object_implements_drawable(UtObject *object);
