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

/// Creates a new Wayland surface object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandSurface
UtObject *ut_wayland_surface_new(UtObject *client, uint32_t id,
                                 UtObject *callback_object,
                                 const UtWaylandSurfaceCallbacks *callbacks);

/// Destroy this surface.
void ut_wayland_surface_destroy(UtObject *object);

/// Attaches [buffer] to this surface.
/// [x],[y] is the location in the buffer of the current upper left corner of
/// the surface.
///
/// !arg-type buffer UtWaylandBuffer
void ut_wayland_surface_attach(UtObject *object, UtObject *buffer, int32_t x,
                               int32_t y);

/// Marks the rectangle of size [width]x[height] at [x],[y] as needing to be
/// redrawn.
void ut_wayland_surface_damage(UtObject *object, int32_t x, int32_t y,
                               int32_t width, int32_t height);

/// Renders a frame on this surface.
/// [done_callback] is called when this render completes.
///
/// !return-ref
/// !return-type UtWaylandCallback
UtObject *ut_wayland_surface_frame(UtObject *object, UtObject *callback_object,
                                   UtWaylandCallbackDoneCallback done_callback);

/// Set the opaque [region] for this surface.
///
/// !arg-type region UtWaylandRegion
void ut_wayland_surface_set_opaque_region(UtObject *object, UtObject *region);

/// Sets the input [region] for this surface.
///
/// !arg-type region UtWaylandRegion
void ut_wayland_surface_set_input_region(UtObject *object, UtObject *region);

/// Commits state changes to this surface.
void ut_wayland_surface_commit(UtObject *object);

/// Sets the [transform] applied the the buffer on this surface.
void ut_wayland_surface_set_buffer_transform(UtObject *object,
                                             int32_t transform);

/// Sets the [scale] applied to the buffer on this surface.
void ut_wayland_surface_set_buffer_scale(UtObject *object, int32_t scale);

/// Marks the rectangle of size [width]x[height] at [x],[y] as needing to be
/// redrawn on this surfaces buffer.
void ut_wayland_surface_damage_buffer(UtObject *object, int32_t x, int32_t y,
                                      int32_t width, int32_t height);

/// Sets the offset for this surface to [x],[y].
void ut_wayland_surface_offset(UtObject *object, int32_t x, int32_t y);

/// Returns [true] if [object] is a [UtWaylandSurface].
bool ut_object_is_wayland_surface(UtObject *object);
