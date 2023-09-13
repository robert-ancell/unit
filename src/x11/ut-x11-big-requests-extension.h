#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11ClientBigRequestsEnableCallback)(
    UtObject *object, uint32_t maximum_request_length, UtObject *error);

/// Creates a new X11 big requests extension.
///
/// !arg-type client UtX11Client
/// !return-ref
/// !return-type UtX11BigRequestsExtension
UtObject *ut_x11_big_requests_extension_new(UtObject *client,
                                            uint8_t major_opcode);

void ut_x11_big_requests_extension_enable(
    UtObject *object, UtObject *callback_object,
    UtX11ClientBigRequestsEnableCallback callback);

/// Returns [true] if [object] is a [UtX11BigRequestsExtension].
bool ut_object_is_x11_big_requests_extension(UtObject *object);
