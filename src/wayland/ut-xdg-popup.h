#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtXdgPopupConfigureCallback)(void *user_data, int32_t x,
                                            int32_t y, int32_t width,
                                            int32_t height);
typedef void (*UtXdgPopupPopupDoneCallback)(void *user_data);
typedef void (*UtXdgPopupRepositionedCallback)(void *user_data, uint32_t token);

typedef struct {
  UtXdgPopupConfigureCallback configure;
  UtXdgPopupPopupDoneCallback popup_done;
  UtXdgPopupRepositionedCallback repositioned;
} UtXdgPopupCallbacks;

UtObject *ut_xdg_popup_new(UtObject *client, uint32_t id,
                           const UtXdgPopupCallbacks *callbacks,
                           void *user_data);

void ut_xdg_popup_destroy(UtObject *object);

bool ut_object_is_xdg_popup(UtObject *object);
