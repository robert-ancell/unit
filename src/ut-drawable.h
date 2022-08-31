#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  void (*clear)(UtObject *object, UtObject *color);
} UtDrawableInterface;

extern int ut_drawable_id;

void ut_drawable_clear(UtObject *object, UtObject *color);

bool ut_object_implements_drawable(UtObject *object);
