#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  size_t (*get_width)(UtObject *object);
  size_t (*get_height)(UtObject *object);
  UtObject *(*get_data)(UtObject *object);
} UtImageBufferInterface;

extern int ut_image_buffer_id;

size_t ut_image_buffer_get_width(UtObject *object);

size_t ut_image_buffer_get_height(UtObject *object);

UtObject *ut_image_buffer_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtImageBuffer].
bool ut_object_implements_image_buffer(UtObject *object);
