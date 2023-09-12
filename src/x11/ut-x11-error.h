#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef enum {
  UT_X11_ERROR_UNKNOWN,
  UT_X11_ERROR_REQUEST,
  UT_X11_ERROR_VALUE,
  UT_X11_ERROR_WINDOW,
  UT_X11_ERROR_PIXMAP,
  UT_X11_ERROR_ATOM,
  UT_X11_ERROR_CURSOR,
  UT_X11_ERROR_FONT,
  UT_X11_ERROR_MATCH,
  UT_X11_ERROR_DRAWABLE,
  UT_X11_ERROR_ACCESS,
  UT_X11_ERROR_ALLOC,
  UT_X11_ERROR_COLORMAP,
  UT_X11_ERROR_GCONTEXT,
  UT_X11_ERROR_ID_CHOICE,
  UT_X11_ERROR_NAME,
  UT_X11_ERROR_LENGTH,
  UT_X11_ERROR_IMPLEMENTATION,
  UT_X11_ERROR_BAD_SEGMENT,
  UT_X11_ERROR_INPUT_DEVICE,
  UT_X11_ERROR_INPUT_EVENT,
  UT_X11_ERROR_INPUT_MODE,
  UT_X11_ERROR_INPUT_DEVICE_BUSY,
  UT_X11_ERROR_INPUT_CLASS,
  UT_X11_ERROR_COUNTER,
  UT_X11_ERROR_ALARM,
  UT_X11_ERROR_BAD_REGION,
  UT_X11_ERROR_BAD_OUTPUT,
  UT_X11_ERROR_BAD_CRTC,
  UT_X11_ERROR_BAD_MODE,
  UT_X11_ERROR_BAD_PROVIDER
} UtX11ErrorCode;

/// Creates a new X11 error of type [code].
/// [value] contains information about this error that depends on the value in
/// [code]. [major_opcode] and [minor_opcode] are the request this error is in
/// response to.
///
/// !return-ref
/// !return-type UtX11Error
UtObject *ut_x11_error_new(UtX11ErrorCode code, uint32_t value,
                           uint8_t major_opcode, uint16_t minor_opcode);

/// Returns [true] if [object] is a [UtX11Error].
bool ut_object_is_x11_error(UtObject *object);
