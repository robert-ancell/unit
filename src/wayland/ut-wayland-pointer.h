#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandPointerEnterCallback)(UtObject *object, uint32_t serial,
                                              UtObject *surface,
                                              double surface_x,
                                              double surface_y);
typedef void (*UtWaylandPointerLeaveCallback)(UtObject *object, uint32_t serial,
                                              UtObject *surface);
typedef void (*UtWaylandPointerMotionCallback)(UtObject *object, uint32_t time,
                                               double surface_x,
                                               double surface_y);
typedef void (*UtWaylandPointerButtonCallback)(UtObject *object,
                                               uint32_t serial, uint32_t time,
                                               uint32_t button, uint32_t state);
typedef void (*UtWaylandPointerFrameCallback)(UtObject *object);

typedef struct {
  UtWaylandPointerEnterCallback enter;
  UtWaylandPointerLeaveCallback leave;
  UtWaylandPointerMotionCallback motion;
  UtWaylandPointerButtonCallback button;
  UtWaylandPointerFrameCallback frame;
} UtWaylandPointerCallbacks;

/// Creates a new Wayland pointer object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandPointer
UtObject *ut_wayland_pointer_new(UtObject *client, uint32_t id,
                                 UtObject *callback_object,
                                 const UtWaylandPointerCallbacks *callbacks);

void ut_wayland_pointer_set_cursor(UtObject *object, uint32_t serial,
                                   UtObject *surface, int32_t hotspot_x,
                                   int32_t hotspot_y);

/// Release this object.
void ut_wayland_pointer_release(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandPointer].
bool ut_object_is_wayland_pointer(UtObject *object);
