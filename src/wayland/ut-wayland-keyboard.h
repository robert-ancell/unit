#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtWaylandKeyboardKeymapCallback)(void *user_data, uint32_t time,
                                                double surface_x,
                                                double surface_y);
typedef void (*UtWaylandKeyboardEnterCallback)(void *user_data, uint32_t serial,
                                               UtObject *surface,
                                               UtObject *keys);
typedef void (*UtWaylandKeyboardLeaveCallback)(void *user_data, uint32_t serial,
                                               UtObject *surface);
typedef void (*UtWaylandKeyboardKeyCallback)(void *user_data, uint32_t serial,
                                             uint32_t time, uint32_t key,
                                             uint32_t state);
typedef void (*UtWaylandKeyboardModifiersCallback)(
    void *user_data, uint32_t serial, uint32_t mods_depressed,
    uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
typedef void (*UtWaylandKeyboardRepeatInfoCallback)(void *user_data,
                                                    int32_t rate,
                                                    int32_t delay);

typedef struct {
  UtWaylandKeyboardKeymapCallback motion;
  UtWaylandKeyboardEnterCallback enter;
  UtWaylandKeyboardLeaveCallback leave;
  UtWaylandKeyboardKeyCallback key;
  UtWaylandKeyboardModifiersCallback modifiers;
  UtWaylandKeyboardRepeatInfoCallback repeat_info;
} UtWaylandKeyboardCallbacks;

UtObject *ut_wayland_keyboard_new(UtObject *client, uint32_t id,
                                  const UtWaylandKeyboardCallbacks *callbacks,
                                  void *user_data);

void ut_wayland_keyboard_release(UtObject *object);

bool ut_object_is_wayland_keyboard(UtObject *object);
