#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandTouchDownCallback)(UtObject *object, uint32_t serial,
                                           uint32_t time, UtObject *surface,
                                           int32_t id, double surface_x,
                                           double surface_y);
typedef void (*UtWaylandTouchUpCallback)(UtObject *object, uint32_t serial,
                                         uint32_t time, int32_t id);
typedef void (*UtWaylandTouchMotionCallback)(UtObject *object, uint32_t time,
                                             int32_t id, double surface_x,
                                             double surface_y);
typedef void (*UtWaylandTouchFrameCallback)(UtObject *object);
typedef void (*UtWaylandTouchCancelCallback)(UtObject *object);
typedef void (*UtWaylandTouchShapeCallback)(UtObject *object, int32_t id,
                                            double major, double minor);
typedef void (*UtWaylandTouchOrientationCallback)(UtObject *object, int32_t id,
                                                  double orientation);

typedef struct {
  UtWaylandTouchDownCallback down;
  UtWaylandTouchUpCallback up;
  UtWaylandTouchMotionCallback motion;
  UtWaylandTouchFrameCallback frame;
  UtWaylandTouchCancelCallback cancel;
  UtWaylandTouchShapeCallback shape;
  UtWaylandTouchOrientationCallback orientation;
} UtWaylandTouchCallbacks;

/// Creates a new Wayland touch object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandTouch
UtObject *ut_wayland_touch_new(UtObject *client, uint32_t id,
                               UtObject *callback_object,
                               const UtWaylandTouchCallbacks *callbacks);

/// Release this object.
void ut_wayland_touch_release(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandTouch].
bool ut_object_is_wayland_touch(UtObject *object);
