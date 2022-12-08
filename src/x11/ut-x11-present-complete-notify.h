#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_present_complete_notify_new(uint8_t kind, uint8_t mode,
                                             uint32_t event_id, uint32_t window,
                                             uint32_t serial, uint64_t ust,
                                             uint64_t msc);

uint8_t ut_x11_present_complete_notify_get_kind(UtObject *object);

uint8_t ut_x11_present_complete_notify_get_mode(UtObject *object);

uint32_t ut_x11_present_complete_notify_get_event_id(UtObject *object);

uint32_t ut_x11_present_complete_notify_get_window(UtObject *object);

uint32_t ut_x11_present_complete_notify_get_serial(UtObject *object);

uint64_t ut_x11_present_complete_notify_get_ust(UtObject *object);

uint64_t ut_x11_present_complete_notify_get_msc(UtObject *object);

bool ut_object_is_x11_present_complete_notify(UtObject *object);
