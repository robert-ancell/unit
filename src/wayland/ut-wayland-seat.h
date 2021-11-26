#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-keyboard.h"
#include "ut-wayland-pointer.h"
#include "ut-wayland-touch.h"

#pragma once

typedef enum {
  UT_WAYLAND_SEAT_CAPABILITY_POINTER = 1 << 0,
  UT_WAYLAND_SEAT_CAPABILITY_KEYBOARD = 1 << 1,
  UT_WAYLAND_SEAT_CAPABILITY_TOUCH = 1 << 2
} UtWaylandSeatCapability;

typedef void (*UtWaylandSeatCapabilitiesCallback)(void *user_data,
                                                  uint32_t capabilities);
typedef void (*UtWaylandSeatNameCallback)(void *user_data, const char *name);

typedef struct {
  UtWaylandSeatCapabilitiesCallback capabilities;
  UtWaylandSeatNameCallback name;
} UtWaylandSeatCallbacks;

UtObject *ut_wayland_seat_new(UtObject *client, uint32_t id,
                              const UtWaylandSeatCallbacks *callbacks,
                              void *user_data);

UtObject *
ut_wayland_seat_new_from_registry(UtObject *registry, uint32_t name,
                                  const UtWaylandSeatCallbacks *callbacks,
                                  void *user_data);

UtObject *
ut_wayland_seat_get_pointer(UtObject *object,
                            const UtWaylandPointerCallbacks *callbacks,
                            void *user_data);

UtObject *
ut_wayland_seat_get_keyboard(UtObject *object,
                             const UtWaylandKeyboardCallbacks *callbacks,
                             void *user_data);

UtObject *ut_wayland_seat_get_touch(UtObject *object,
                                    const UtWaylandTouchCallbacks *callbacks,
                                    void *user_data);

void ut_wayland_seat_release(UtObject *object);

bool ut_object_is_wayland_seat(UtObject *object);
