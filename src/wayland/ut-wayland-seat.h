#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-wayland-keyboard.h"
#include "ut-wayland-pointer.h"
#include "ut-wayland-touch.h"

#pragma once

/// Capabilities of this seat:
/// - [UT_WAYLAND_SEAT_CAPABILITY_POINTER] - Has pointer input.
/// - [UT_WAYLAND_SEAT_CAPABILITY_KEYBOARD] - Has keyboard input.
/// - [UT_WAYLAND_SEAT_CAPABILITY_TOUCH] - Has touch input.
typedef enum {
  UT_WAYLAND_SEAT_CAPABILITY_POINTER = 1 << 0,
  UT_WAYLAND_SEAT_CAPABILITY_KEYBOARD = 1 << 1,
  UT_WAYLAND_SEAT_CAPABILITY_TOUCH = 1 << 2
} UtWaylandSeatCapability;

typedef void (*UtWaylandSeatCapabilitiesCallback)(UtObject *object,
                                                  uint32_t capabilities);
typedef void (*UtWaylandSeatNameCallback)(UtObject *object, const char *name);

typedef struct {
  UtWaylandSeatCapabilitiesCallback capabilities;
  UtWaylandSeatNameCallback name;
} UtWaylandSeatCallbacks;

/// Creates a new Wayland seat object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtWaylandSeat
UtObject *ut_wayland_seat_new(UtObject *client, uint32_t id,
                              UtObject *callback_object,
                              const UtWaylandSeatCallbacks *callbacks);

/// Creates a new Wayland seat object with well known [name] from [registry].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type registry UtWaylandRegistry
/// !return-ref
/// !return-type UtWaylandSeat
UtObject *
ut_wayland_seat_new_from_registry(UtObject *registry, uint32_t name,
                                  UtObject *callback_object,
                                  const UtWaylandSeatCallbacks *callbacks);

/// Returns the pointer object.
/// The functions in [callbacks] will be called for any events for this object.
///
/// !return-ref
/// !return-type UtWaylandPointer
UtObject *
ut_wayland_seat_get_pointer(UtObject *object, UtObject *callback_object,
                            const UtWaylandPointerCallbacks *callbacks);

/// Returns the keyboard object.
/// The functions in [callbacks] will be called for any events for this object.
///
/// !return-ref
/// !return-type UtWaylandKeyboard
UtObject *
ut_wayland_seat_get_keyboard(UtObject *object, UtObject *callback_object,
                             const UtWaylandKeyboardCallbacks *callbacks);

/// Returns the touch object.
/// The functions in [callbacks] will be called for any events for this object.
///
/// !return-ref
/// !return-type UtWaylandTouch
UtObject *ut_wayland_seat_get_touch(UtObject *object, UtObject *callback_object,
                                    const UtWaylandTouchCallbacks *callbacks);

/// Release this object.
void ut_wayland_seat_release(UtObject *object);

/// Returns [true] if [object] is a [UtWaylandSeat].
bool ut_object_is_wayland_seat(UtObject *object);
