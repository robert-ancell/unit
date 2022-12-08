#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_x11_present_configure_notify_new(
    uint32_t event_id, uint32_t window, int16_t x, int16_t y, uint16_t width,
    uint16_t height, int16_t off_x, int16_t off_y, uint16_t pixmap_width,
    uint16_t pixmap_height, uint32_t pixmap_flags);

uint32_t ut_x11_present_configure_notify_get_event_id(UtObject *object);

uint32_t ut_x11_present_configure_notify_get_window(UtObject *object);

int16_t ut_x11_present_configure_notify_get_x(UtObject *object);

int16_t ut_x11_present_configure_notify_get_y(UtObject *object);

uint16_t ut_x11_present_configure_notify_get_width(UtObject *object);

uint16_t ut_x11_present_configure_notify_get_height(UtObject *object);

int16_t ut_x11_present_configure_notify_get_off_x(UtObject *object);

int16_t ut_x11_present_configure_notify_get_off_y(UtObject *object);

uint16_t ut_x11_present_configure_notify_get_pixmap_width(UtObject *object);

uint16_t ut_x11_present_configure_notify_get_pixmap_height(UtObject *object);

uint32_t ut_x11_present_configure_notify_get_pixmap_flags(UtObject *object);

bool ut_object_is_x11_present_configure_notify(UtObject *object);
