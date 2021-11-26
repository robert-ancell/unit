#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandTouchDownCallback)(void *user_data, uint32_t serial,
                                           uint32_t time, UtObject *surface,
                                           int32_t id, double surface_x,
                                           double surface_y);
typedef void (*UtWaylandTouchUpCallback)(void *user_data, uint32_t serial,
                                         uint32_t time, int32_t id);
typedef void (*UtWaylandTouchMotionCallback)(void *user_data, uint32_t time,
                                             int32_t id, double surface_x,
                                             double surface_y);
typedef void (*UtWaylandTouchFrameCallback)(void *user_data);
typedef void (*UtWaylandTouchCancelCallback)(void *user_data);
typedef void (*UtWaylandTouchShapeCallback)(void *user_data, int32_t id,
                                            double major, double minor);
typedef void (*UtWaylandTouchOrientationCallback)(void *user_data, int32_t id,
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

UtObject *ut_wayland_touch_new(UtObject *client, uint32_t id,
                               const UtWaylandTouchCallbacks *callbacks,
                               void *user_data);

void ut_wayland_touch_release(UtObject *object);

bool ut_object_is_wayland_touch(UtObject *object);
