#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientRandrQueryVersionCallback)(void *user_data,
                                                     uint32_t major_version,
                                                     uint32_t minor_version,
                                                     UtObject *error);

UtObject *ut_x11_randr_extension_new(UtObject *client, uint8_t major_opcode,
                                     uint8_t first_event, uint8_t first_error,
                                     const UtX11EventCallbacks *event_callbacks,
                                     void *user_data, UtObject *cancel);

void ut_x11_randr_extension_query_version(
    UtObject *object, UtX11ClientRandrQueryVersionCallback callback,
    void *user_data, UtObject *cancel);

bool ut_object_is_x11_randr_extension(UtObject *object);
