#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandRegistryGlobalCallback)(UtObject *object, uint32_t name,
                                                const char *interface,
                                                uint32_t version);
typedef void (*UtWaylandRegistryGlobalRemoveCallback)(UtObject *object,
                                                      uint32_t name);

typedef struct {
  UtWaylandRegistryGlobalCallback global;
  UtWaylandRegistryGlobalRemoveCallback global_remove;
} UtWaylandRegistryCallbacks;

/// Creates a new Wayland registry object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandRegistry
UtObject *ut_wayland_registry_new(UtObject *client, uint32_t id,
                                  UtObject *callback_object,
                                  const UtWaylandRegistryCallbacks *callbacks);

/// Returns a new object ID that is bound to the well known [name].
/// The object must match [interface] and [version].
uint32_t ut_wayland_registry_bind(UtObject *object, uint32_t name,
                                  const char *interface, uint32_t version);

/// Returns [true] if [object] is a [UtWaylandRegistry].
bool ut_object_is_wayland_registry(UtObject *object);
