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

UtObject *ut_wayland_registry_new(UtObject *client, uint32_t id,
                                  UtObject *callback_object,
                                  const UtWaylandRegistryCallbacks *callbacks);

uint32_t ut_wayland_registry_bind(UtObject *object, uint32_t name,
                                  const char *interface, uint32_t version);

/// Returns [true] if [object] is a [UtWaylandRegistry].
bool ut_object_is_wayland_registry(UtObject *object);
