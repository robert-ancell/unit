#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtXdgWmBasePingCallback)(void *user_data, uint32_t serial);

typedef struct {
  UtXdgWmBasePingCallback ping;
} UtXdgWmBaseCallbacks;

UtObject *ut_xdg_wm_base_new(UtObject *client, uint32_t id,
                             const UtXdgWmBaseCallbacks *callbacks,
                             void *user_data);

UtObject *
ut_xdg_wm_base_new_from_registry(UtObject *registry, uint32_t name,
                                 const UtXdgWmBaseCallbacks *callbacks,
                                 void *user_data);

void ut_xdg_wm_base_destroy(UtObject *object);

UtObject *ut_xdg_wm_base_create_positioner(UtObject *object);

UtObject *ut_xdg_wm_base_get_xdg_surface(UtObject *object, UtObject *surface,
                                         const UtXdgSurfaceCallbacks *callback,
                                         void *user_data);

void ut_xdg_wm_base_pong(UtObject *object, uint32_t serial);

bool ut_object_is_xdg_wm_base(UtObject *object);
