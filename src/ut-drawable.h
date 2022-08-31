#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*clear)(UtObject *object, double red, double green, double blue,
                double alpha);
} UtDrawableInterface;

extern int ut_drawable_id;

void ut_drawable_clear(UtObject *object, double red, double green, double blue,
                       double alpha);

bool ut_object_implements_drawable(UtObject *object);
