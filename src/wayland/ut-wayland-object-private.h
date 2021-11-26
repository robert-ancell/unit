#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

bool ut_wayland_object_decode_event(UtObject *object, uint16_t code,
                                    UtObject *data);
