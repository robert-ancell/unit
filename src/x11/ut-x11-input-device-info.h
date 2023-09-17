#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new X11 device info.
///
/// !arg-type classes UtObjectList
/// !return-ref
/// !return-type UtX11DeviceInfo
UtObject *ut_x11_input_device_info_new(uint32_t id, const char *name,
                                       uint16_t type, uint32_t attachment,
                                       UtObject *classes, bool enabled);

/// Returns the ID for this input device.
uint32_t ut_x11_input_device_info_get_id(UtObject *object);

/// Returns the name of this input device.
const char *ut_x11_input_device_info_get_name(UtObject *object);

/// Returns the type of input device this is.
uint16_t ut_x11_input_device_info_get_type(UtObject *object);

/// Returns the device this is attached to.
uint32_t ut_x11_input_device_info_get_attachment(UtObject *object);

/// Returns the classes this input device uses.
///
/// !return-type UtObjectList
UtObject *ut_x11_input_device_info_get_classes(UtObject *object);

/// Returns true if this input device is enabled.
bool ut_x11_input_device_info_get_enabled(UtObject *object);

/// Returns [true] if [object] is a [UtX11InputDeviceInfo].
bool ut_object_is_x11_input_device_info(UtObject *object);
