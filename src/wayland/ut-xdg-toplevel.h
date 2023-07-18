#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtXdgToplevelConfigureCallback)(UtObject *object, int32_t width,
                                               int32_t height,
                                               UtObject *states);
typedef void (*UtXdgToplevelCloseCallback)(UtObject *object);
typedef void (*UtXdgToplevelConfigureBoundsCallback)(UtObject *object,
                                                     int32_t width,
                                                     int32_t height);
typedef void (*UtXdgToplevelWmCapabilitiesCallback)(UtObject *object,
                                                    UtObject *capabilities);

typedef struct {
  UtXdgToplevelConfigureCallback configure;
  UtXdgToplevelCloseCallback close;
  UtXdgToplevelConfigureBoundsCallback configure_bounds;
  UtXdgToplevelWmCapabilitiesCallback wm_capabilities;
} UtXdgToplevelCallbacks;

UtObject *ut_xdg_toplevel_new(UtObject *client, uint32_t id,
                              UtObject *callback_object,
                              const UtXdgToplevelCallbacks *callbacks);

void ut_xdg_toplevel_destroy(UtObject *object);

void ut_xdg_toplevel_set_parent(UtObject *object, UtObject *parent);

void ut_xdg_toplevel_set_title(UtObject *object, const char *title);

void ut_xdg_toplevel_set_app_id(UtObject *object, const char *app_id);

void ut_xdg_toplevel_show_window_menu(UtObject *object, UtObject *seat,
                                      uint32_t serial, int32_t x, int32_t y);

void ut_xdg_toplevel_move(UtObject *object, UtObject *seat, uint32_t serial);

void ut_xdg_toplevel_resize(UtObject *object, UtObject *seat, uint32_t serial,
                            uint32_t edge);

void ut_xdg_toplevel_set_max_size(UtObject *object, int32_t width,
                                  int32_t height);

void ut_xdg_toplevel_set_min_size(UtObject *object, int32_t width,
                                  int32_t height);

void ut_xdg_toplevel_set_maximized(UtObject *object);

void ut_xdg_toplevel_unset_maximized(UtObject *object);

void ut_xdg_toplevel_set_fullscreen(UtObject *object, UtObject *output);

void ut_xdg_toplevel_unset_fullscreen(UtObject *object);

void ut_xdg_toplevel_set_minimized(UtObject *object);

bool ut_object_is_xdg_toplevel(UtObject *object);
