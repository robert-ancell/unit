#include <stdint.h>

#include "ut-object.h"

#pragma once

uint32_t ut_wayland_client_allocate_id(UtObject *object);

void ut_wayland_client_register_object(UtObject *object,
                                       UtObject *wayland_object);

UtObject *ut_wayland_client_get_object(UtObject *object, uint32_t id);

void ut_wayland_client_send_request(UtObject *object, uint32_t id,
                                    uint16_t code, UtObject *payload);
