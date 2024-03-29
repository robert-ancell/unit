#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientRandrQueryVersionCallback)(UtObject *object,
                                                     uint32_t major_version,
                                                     uint32_t minor_version,
                                                     UtObject *error);

/// Creates a new X11 RANDR extension.
///
/// !arg-type client UtX11Client
/// !return-ref
/// !return-type UtX11RandrExtension
UtObject *
ut_x11_randr_extension_new(UtObject *client, uint8_t major_opcode,
                           uint8_t first_event, uint8_t first_error,
                           UtObject *callback_object,
                           const UtX11EventCallbacks *event_callbacks);

void ut_x11_randr_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11ClientRandrQueryVersionCallback callback);

/// Returns [true] if [object] is a [UtX11RandrExtension].
bool ut_object_is_x11_randr_extension(UtObject *object);
