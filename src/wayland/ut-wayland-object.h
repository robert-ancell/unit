#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  const char *(*get_interface)(UtObject *object);
  uint32_t (*get_id)(UtObject *object);
  bool (*decode_event)(UtObject *object, uint16_t code, UtObject *data);
} UtWaylandObjectInterface;

extern int ut_wayland_object_id;

/// Returns the interface this object is using.
const char *ut_wayland_object_get_interface(UtObject *object);

/// Returns the ID for this object.
uint32_t ut_wayland_object_get_id(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandObject].
bool ut_object_implements_wayland_object(UtObject *object);
