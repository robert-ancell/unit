#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_present_idle_notify_new(uint32_t event_id, uint32_t window,
                                         uint32_t serial, uint32_t pixmap,
                                         uint32_t idle_fence);

uint32_t ut_x11_present_idle_notify_get_event_id(UtObject *object);

uint32_t ut_x11_present_idle_notify_get_window(UtObject *object);

uint32_t ut_x11_present_idle_notify_get_serial(UtObject *object);

uint32_t ut_x11_present_idle_notify_get_pixmap(UtObject *object);

uint32_t ut_x11_present_idle_notify_get_idle_fence(UtObject *object);

bool ut_object_is_x11_present_idle_notify(UtObject *object);
