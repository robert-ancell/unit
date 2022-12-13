#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_input_event_mask_new(uint16_t device_id, uint64_t value);

uint16_t ut_x11_input_event_mask_get_device_id(UtObject *object);

uint64_t ut_x11_input_event_mask_get_value(UtObject *object);

bool ut_object_is_x11_input_event_mask(UtObject *object);
