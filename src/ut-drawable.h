#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*clear)(UtObject *object, UtObject *color);
  void (*render_box)(UtObject *object, double x, double y, double width,
                     double height, UtObject *color);
} UtDrawableInterface;

extern int ut_drawable_id;

void ut_drawable_clear(UtObject *object, UtObject *color);

void ut_drawable_render_box(UtObject *object, double x, double y, double width,
                            double height, UtObject *color);

/// Returns [true] if [object] is a [UtDrawable].
bool ut_object_implements_drawable(UtObject *object);
