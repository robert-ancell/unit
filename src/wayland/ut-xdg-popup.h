#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtXdgPopupConfigureCallback)(UtObject *object, int32_t x,
                                            int32_t y, int32_t width,
                                            int32_t height);
typedef void (*UtXdgPopupPopupDoneCallback)(UtObject *object);
typedef void (*UtXdgPopupRepositionedCallback)(UtObject *object,
                                               uint32_t token);

typedef struct {
  UtXdgPopupConfigureCallback configure;
  UtXdgPopupPopupDoneCallback popup_done;
  UtXdgPopupRepositionedCallback repositioned;
} UtXdgPopupCallbacks;

/// Creates a new XDG popup object with [id] on [client].
/// The functions in [callbacks] will be called for any events for this object.
///
/// !arg-type client UtWaylandClient
/// !return-ref
/// !return-type UtXdgPopup
UtObject *ut_xdg_popup_new(UtObject *client, uint32_t id,
                           UtObject *callback_object,
                           const UtXdgPopupCallbacks *callbacks);

/// Destroy this popup.
void ut_xdg_popup_destroy(UtObject *object);

/// Returns [true] if [object] is a [UtXdgPopup].
bool ut_object_is_xdg_popup(UtObject *object);
