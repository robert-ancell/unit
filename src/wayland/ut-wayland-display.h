#include <stdbool.h>

#include "ut-object.h"
#include "ut-wayland-callback.h"
#include "ut-wayland-registry.h"

#pragma once

typedef void (*UtWaylandDisplayErrorCallback)(UtObject *object, uint32_t id,
                                              uint32_t code,
                                              const char *message);
typedef void (*UtWaylandDisplayDeleteIdCallback)(UtObject *object, uint32_t id);

typedef struct {
  UtWaylandDisplayErrorCallback error;
  UtWaylandDisplayDeleteIdCallback delete_id;
} UtWaylandDisplayCallbacks;

/// Creates a new Wayland display object on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandDisplay
UtObject *ut_wayland_display_new(UtObject *client, UtObject *callback_object,
                                 const UtWaylandDisplayCallbacks *callbacks);

/// Makes a sync request to the server.
/// [done_callback] will be called when this sync is complete.
///
/// !return-ref
/// !return-type UtWaylandCallback
UtObject *ut_wayland_display_sync(UtObject *object, UtObject *callback_object,
                                  UtWaylandCallbackDoneCallback done_callback);

/// Returns the registry for this display.
/// The functions in [callbacks] will be called for any events for this
/// registry.
///
/// !return-ref
/// !return-type UtWaylandRegistry
UtObject *
ut_wayland_display_get_registry(UtObject *object, UtObject *callback_object,
                                const UtWaylandRegistryCallbacks *callbacks);

/// Returns [true] if [object] is a [UtWaylandDisplay].
bool ut_object_is_wayland_display(UtObject *object);
