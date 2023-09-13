#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11XinputQueryVersionCallback)(UtObject *object,
                                                uint16_t major_version,
                                                uint16_t minor_version,
                                                UtObject *error);

UtObject *
ut_x11_xinput_extension_new(UtObject *client, uint8_t major_opcode,
                            uint8_t first_event, uint8_t first_error,
                            UtObject *callback_object,
                            const UtX11EventCallbacks *event_callbacks);

void ut_x11_xinput_extension_select_events(UtObject *object, uint32_t window,
                                           UtObject *masks);

void ut_x11_xinput_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11XinputQueryVersionCallback callback);

void ut_x11_xinput_extension_query_device(UtObject *object, uint16_t device_id,
                                          UtObject *callback_object,
                                          UtX11QueryDeviceCallback callback);

void ut_x11_xinput_extension_set_focus(UtObject *object, uint32_t window,
                                       uint32_t timestamp, uint16_t device_id);

void ut_x11_xinput_extension_get_focus(UtObject *object, uint16_t device_id,
                                       UtObject *callback_object,
                                       UtX11GetFocusCallback callback);

void ut_x11_xinput_extension_grab_device(UtObject *object, uint32_t window,
                                         uint32_t timestamp, uint32_t cursor,
                                         uint16_t device_id, uint8_t model,
                                         uint8_t paired_device_mode,
                                         bool owner_events, UtObject *masks,
                                         UtObject *callback_object,
                                         UtX11GrabDeviceCallback callback);

void ut_x11_xinput_extension_ungrab_device(UtObject *object,
                                           uint16_t device_id);

// AllowEvents

// PassiveGrabDevice

// PassiveUngrabDevice

// ListProperties

// ChangeProperties

// DeleteProperties

// GetProperty

// GetSelectedEvents

/// Returns [true] if [object] is a [UtX11XinputExtension].
bool ut_object_is_x11_xinput_extension(UtObject *object);
