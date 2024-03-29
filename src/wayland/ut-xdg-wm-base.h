#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtXdgWmBasePingCallback)(UtObject *object, uint32_t serial);

typedef struct {
  UtXdgWmBasePingCallback ping;
} UtXdgWmBaseCallbacks;

/// Creates a new XDG WM base extension object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtXdgWmBase
UtObject *ut_xdg_wm_base_new(UtObject *client, uint32_t id,
                             UtObject *callback_object,
                             const UtXdgWmBaseCallbacks *callbacks);

/// Creates a new XDG WM base extension object with well known [name] from
/// [registry]. The functions in [callbacks] will be called for any events for
/// this object.
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtXdgWmBase
UtObject *
ut_xdg_wm_base_new_from_registry(UtObject *registry, uint32_t name,
                                 UtObject *callback_object,
                                 const UtXdgWmBaseCallbacks *callbacks);

/// Destroy this object.
void ut_xdg_wm_base_destroy(UtObject *object);

/// Returns a new positioner.
///
/// !return-ref
/// !return-type UtXdgPositioner
UtObject *ut_xdg_wm_base_create_positioner(UtObject *object);

/// Returns a new XDG surface for [surface].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type surface UtWaylandSurface
/// !return-ref
/// !return-type UtXdgSurface
UtObject *
ut_xdg_wm_base_get_xdg_surface(UtObject *object, UtObject *surface,
                               UtObject *callback_object,
                               const UtXdgSurfaceCallbacks *callbacks);

/// Response to a ping with [serial].
void ut_xdg_wm_base_pong(UtObject *object, uint32_t serial);

/// Returns [true] if [object] is a [UtXdgWmBase].
bool ut_object_is_xdg_wm_base(UtObject *object);
