#include <stdbool.h>

#include "ut-object.h"
#include "ut-wayland-callback.h"
#include "ut-wayland-registry.h"

#pragma once

typedef void (*UtWaylandDisplayErrorCallback)(void *user_data, uint32_t id,
                                              uint32_t code,
                                              const char *message);
typedef void (*UtWaylandDisplayDeleteIdCallback)(void *user_data, uint32_t id);

typedef struct {
  UtWaylandDisplayErrorCallback error;
  UtWaylandDisplayDeleteIdCallback delete_id;
} UtWaylandDisplayCallbacks;

UtObject *ut_wayland_display_new(UtObject *client,
                                 const UtWaylandDisplayCallbacks *callbacks,
                                 void *user_data);

UtObject *ut_wayland_display_sync(UtObject *object,
                                  UtWaylandCallbackDoneCallback done_callback,
                                  void *user_data);

UtObject *
ut_wayland_display_get_registry(UtObject *object,
                                const UtWaylandRegistryCallbacks *callbacks,
                                void *user_data);

bool ut_object_is_wayland_display(UtObject *object);
