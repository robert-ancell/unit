#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-xdg-popup.h"
#include "ut-xdg-toplevel.h"

#pragma once

typedef void (*UtXdgSurfaceConfigureCallback)(void *user_data, uint32_t serial);

typedef struct {
  UtXdgSurfaceConfigureCallback configure;
} UtXdgSurfaceCallbacks;

UtObject *ut_xdg_surface_new(UtObject *client, uint32_t id,
                             const UtXdgSurfaceCallbacks *callbacks,
                             void *user_data);

void ut_xdg_surface_destroy(UtObject *object);

UtObject *ut_xdg_surface_get_toplevel(UtObject *object,
                                      const UtXdgToplevelCallbacks *callbacks,
                                      void *user_data);

UtObject *ut_xdg_surface_get_popup(UtObject *object, UtObject *parent,
                                   UtObject *positioner,
                                   const UtXdgPopupCallbacks *callbacks,
                                   void *user_data);

void ut_xdg_surface_set_window_geometry(UtObject *object, int32_t x, int32_t y,
                                        int32_t width, int32_t height);

void ut_xdg_surface_ack_configure(UtObject *object, uint32_t serial);

bool ut_object_is_xdg_surface(UtObject *object);
