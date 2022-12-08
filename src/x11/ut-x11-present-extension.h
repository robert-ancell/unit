#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_X11_PRESENT_CONFIGURE_NOTIFY_MASK = 0x1,
  UT_X11_PRESENT_COMPLETE_NOTIFY_MASK = 0x2,
  UT_X11_PRESENT_IDLE_NOTIFY_MASK = 0x4
} UtX11PresentMask;

typedef void (*UtX11ClientPresentQueryVersionCallback)(void *user_data,
                                                       uint32_t major_version,
                                                       uint32_t minor_version,
                                                       UtObject *error);

UtObject *ut_x11_present_extension_new(UtObject *client, uint8_t major_opcode);

void ut_x11_present_extension_query_version(
    UtObject *object, UtX11ClientPresentQueryVersionCallback callback,
    void *user_data, UtObject *cancel);

void ut_x11_present_extension_pixmap(UtObject *object, uint32_t window,
                                     uint32_t pixmap, uint32_t serial);

void ut_x11_present_extension_notify_msc(UtObject *object, uint32_t window,
                                         uint32_t serial, uint64_t target_msc,
                                         uint64_t divisor, uint64_t remainder);

uint32_t ut_x11_present_extension_select_input(UtObject *object,
                                               uint32_t window,
                                               uint32_t event_mask);

void ut_x11_present_extension_query_capabilities(UtObject *object,
                                                 uint32_t crtc_or_window);

bool ut_object_is_x11_present_extension(UtObject *object);
