#include <stdbool.h>

#include "ut-object.h"
#include "ut-wayland-callback.h"
#include "ut-wayland-registry.h"

#pragma once

/// Creates a new [UtWaylandClient] object.
/// !return-ref
UtObject *ut_wayland_client_new();

void ut_wayland_client_connect(UtObject *object);

UtObject *ut_wayland_client_sync(UtObject *object,
                                 UtWaylandCallbackDoneCallback done_callback,
                                 void *user_data);

UtObject *
ut_wayland_client_get_registry(UtObject *object,
                               const UtWaylandRegistryCallbacks *callbacks,
                               void *user_data);

bool ut_object_is_wayland_client(UtObject *object);
