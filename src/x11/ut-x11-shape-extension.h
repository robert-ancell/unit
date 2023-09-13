#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11ShapeNotifyCallback)(UtObject *object, uint32_t window,
                                         uint8_t kind, int16_t x, int16_t y,
                                         uint16_t width, uint16_t height,
                                         uint32_t timestamp, bool shaped);

typedef struct {
  UtX11ShapeNotifyCallback shape_notify;
} UtX11ShapeEventCallbacks;

typedef void (*UtX11ShapeQueryVersionCallback)(UtObject *object,
                                               uint16_t major_version,
                                               uint16_t minor_version,
                                               UtObject *error);

UtObject *
ut_x11_shape_extension_new(UtObject *client, uint8_t major_opcode,
                           uint8_t first_event, UtObject *callback_object,
                           const UtX11ShapeEventCallbacks *event_callbacks);

void ut_x11_shape_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11ShapeQueryVersionCallback callback);

void ut_x11_shape_extension_select_input(UtObject *object, uint32_t window,
                                         bool enabled);

/// Returns [true] if [object] is a [UtX11ShapeExtension].
bool ut_object_is_x11_shape_extension(UtObject *object);
