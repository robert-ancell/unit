#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*clear)(UtObject *object, UtObject *color);
  void (*render_mesh)(UtObject *object, UtObject *mesh, UtObject *color);
} UtDrawableInterface;

extern int ut_drawable_id;

/// Clear the drawable to [color].
///
/// !arg-type color UtColor
void ut_drawable_clear(UtObject *object, UtObject *color);

/// Draw a filled box of size [width]x[height] at [x],[y] filled with [color].
///
/// !arg-type color UtColor
void ut_drawable_render_box(UtObject *object, float x, float y, float width,
                            float height, UtObject *color);

/// Draws a [mesh] filled with [color].
///
/// !arg-type mesh UtMesh
/// !arg-type color UtColor
void ut_drawable_render_mesh(UtObject *object, UtObject *mesh, UtObject *color);

/// Returns [true] if [object] is a [UtDrawable].
bool ut_object_implements_drawable(UtObject *object);
