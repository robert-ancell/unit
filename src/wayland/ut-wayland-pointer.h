#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandPointerEnterCallback)(void *user_data, uint32_t serial,
                                              UtObject *surface,
                                              double surface_x,
                                              double surface_y);
typedef void (*UtWaylandPointerLeaveCallback)(void *user_data, uint32_t serial,
                                              UtObject *surface);
typedef void (*UtWaylandPointerMotionCallback)(void *user_data, uint32_t time,
                                               double surface_x,
                                               double surface_y);
typedef void (*UtWaylandPointerButtonCallback)(void *user_data, uint32_t serial,
                                               uint32_t time, uint32_t button,
                                               uint32_t state);
typedef void (*UtWaylandPointerFrameCallback)(void *user_data);

typedef struct {
  UtWaylandPointerEnterCallback enter;
  UtWaylandPointerLeaveCallback leave;
  UtWaylandPointerMotionCallback motion;
  UtWaylandPointerButtonCallback button;
  UtWaylandPointerFrameCallback frame;
} UtWaylandPointerCallbacks;

UtObject *ut_wayland_pointer_new(UtObject *client, uint32_t id,
                                 const UtWaylandPointerCallbacks *callbacks,
                                 void *user_data);

void ut_wayland_pointer_set_cursor(UtObject *object, uint32_t serial,
                                   UtObject *surface, int32_t hotspot_x,
                                   int32_t hotspot_y);

void ut_wayland_pointer_release(UtObject *object);

bool ut_object_is_wayland_pointer(UtObject *object);
