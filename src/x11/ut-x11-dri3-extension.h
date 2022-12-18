#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientDri3QueryVersionCallback)(void *user_data,
                                                    uint32_t major_version,
                                                    uint32_t minor_version,
                                                    UtObject *error);

UtObject *ut_x11_dri3_extension_new(UtObject *client, uint8_t major_opcode);

void ut_x11_dri3_extension_query_version(
    UtObject *object, UtX11ClientDri3QueryVersionCallback callback,
    void *user_data, UtObject *cancel);

bool ut_object_is_x11_dri3_extension(UtObject *object);