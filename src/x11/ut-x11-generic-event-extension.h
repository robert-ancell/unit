#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11ClientGenericEventQueryVersionCallback)(
    void *user_data, uint16_t major_version, uint16_t minor_version,
    UtObject *error);

UtObject *ut_x11_generic_event_extension_new(UtObject *client,
                                             uint8_t major_opcode);

void ut_x11_generic_event_extension_query_version(
    UtObject *object, UtX11ClientGenericEventQueryVersionCallback callback,
    void *user_data, UtObject *cancel);

bool ut_object_is_x11_generic_event_extension(UtObject *object);