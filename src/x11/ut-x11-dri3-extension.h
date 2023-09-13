#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientDri3QueryVersionCallback)(UtObject *object,
                                                    uint32_t major_version,
                                                    uint32_t minor_version,
                                                    UtObject *error);

/// Creates a new X11 DRI3 extension.
///
/// !return-ref
/// !return-type UtX11Dri3Extension
UtObject *ut_x11_dri3_extension_new(UtObject *client, uint8_t major_opcode);

void ut_x11_dri3_extension_query_version(
    UtObject *object, UtObject *callbac_object,
    UtX11ClientDri3QueryVersionCallback callback);

/// Returns [true] if [object] is a [UtX11Dri3Extension].
bool ut_object_is_x11_dri3_extension(UtObject *object);
