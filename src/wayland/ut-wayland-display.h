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

UtObject *ut_wayland_display_new(UtObject *client, UtObject *callback_object,
                                 const UtWaylandDisplayCallbacks *callbacks);

UtObject *ut_wayland_display_sync(UtObject *object, UtObject *callback_object,
                                  UtWaylandCallbackDoneCallback done_callback);

UtObject *
ut_wayland_display_get_registry(UtObject *object, UtObject *callback_object,
                                const UtWaylandRegistryCallbacks *callbacks);

bool ut_object_is_wayland_display(UtObject *object);
