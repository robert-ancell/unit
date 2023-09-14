#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandCallbackDoneCallback)(UtObject *object,
                                              uint32_t callback_data);

/// Creates a new Wayland callback object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandCallback
UtObject *ut_wayland_callback_new(UtObject *client, uint32_t id,
                                  UtObject *callback_object,
                                  UtWaylandCallbackDoneCallback done_callback);

/// Returns [true] if [object] is a [UtWaylandCallback].
bool ut_object_is_wayland_callback(UtObject *object);
