#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-callback.h"

#pragma once

typedef void (*UtWaylandSurfaceEnterCallback)(UtObject *object,
                                              UtObject *output);
typedef void (*UtWaylandSurfaceLeaveCallback)(UtObject *object,
                                              UtObject *output);

typedef struct {
  UtWaylandSurfaceEnterCallback enter;
  UtWaylandSurfaceLeaveCallback leave;
} UtWaylandSurfaceCallbacks;

/// Creates a new Wayland surface object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandSurface
UtObject *ut_wayland_surface_new(UtObject *client, uint32_t id,
                                 UtObject *callback_object,
                                 const UtWaylandSurfaceCallbacks *callbacks);

/// Destroy this surface.
void ut_wayland_surface_destroy(UtObject *object);

void ut_wayland_surface_attach(UtObject *object, UtObject *buffer, int32_t x,
                               int32_t y);

void ut_wayland_surface_damage(UtObject *object, int32_t x, int32_t y,
                               int32_t width, int32_t height);

UtObject *ut_wayland_surface_frame(UtObject *object, UtObject *callback_object,
                                   UtWaylandCallbackDoneCallback done_callback);

void ut_wayland_surface_set_opaque_region(UtObject *object, UtObject *region);

void ut_wayland_surface_set_input_region(UtObject *object, UtObject *region);

void ut_wayland_surface_commit(UtObject *object);

void ut_wayland_surface_set_buffer_transform(UtObject *object,
                                             int32_t transform);

void ut_wayland_surface_set_buffer_scale(UtObject *object, int32_t scale);

void ut_wayland_surface_damage_buffer(UtObject *object, int32_t x, int32_t y,
                                      int32_t width, int32_t height);

void ut_wayland_surface_offset(UtObject *object, int32_t x, int32_t y);

/// Returns [true] if [object] is a [UtWaylandSurface].
bool ut_object_is_wayland_surface(UtObject *object);
