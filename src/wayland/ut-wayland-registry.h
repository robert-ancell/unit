#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandRegistryGlobalCallback)(void *user_data, uint32_t name,
                                                const char *interface,
                                                uint32_t version);
typedef void (*UtWaylandRegistryGlobalRemoveCallback)(void *user_data,
                                                      uint32_t name);

typedef struct {
  UtWaylandRegistryGlobalCallback global;
  UtWaylandRegistryGlobalRemoveCallback global_remove;
} UtWaylandRegistryCallbacks;

UtObject *ut_wayland_registry_new(UtObject *client, uint32_t id,
                                  const UtWaylandRegistryCallbacks *callbacks,
                                  void *user_data);

uint32_t ut_wayland_registry_bind(UtObject *object, uint32_t name,
                                  const char *interface, uint32_t version);

bool ut_object_is_wayland_registry(UtObject *object);
