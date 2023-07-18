#include <stdbool.h>

#include "ut-object.h"
#include "ut-wayland-callback.h"
#include "ut-wayland-registry.h"

#pragma once

/// Creates a new [UtWaylandClient] object.
///
/// !return-type UtWaylandClient
/// !return-ref
UtObject *ut_wayland_client_new();

/// Connect the [object] to the Wayland server.
void ut_wayland_client_connect(UtObject *object);

UtObject *ut_wayland_client_sync(UtObject *object, UtObject *callback_object,
                                 UtWaylandCallbackDoneCallback done_callback);

UtObject *
ut_wayland_client_get_registry(UtObject *object, UtObject *callback_object,
                               const UtWaylandRegistryCallbacks *callbacks);

bool ut_object_is_wayland_client(UtObject *object);
