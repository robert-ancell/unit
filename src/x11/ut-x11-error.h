#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// X11 error codes:
/// - [UT_X11_ERROR_UNKNOWN] - Unknown error.
/// - [UT_X11_ERROR_REQUEST] - Request error.
/// - [UT_X11_ERROR_VALUE] - Value error.
/// - [UT_X11_ERROR_WINDOW] - Window error.
/// - [UT_X11_ERROR_PIXMAP] - Pixmap error.
/// - [UT_X11_ERROR_ATOM] - Atom error.
/// - [UT_X11_ERROR_CURSOR] - Cursor error.
/// - [UT_X11_ERROR_FONT] - Font error.
/// - [UT_X11_ERROR_MATCH] - Match error.
/// - [UT_X11_ERROR_DRAWABLE] - Drawable error.
/// - [UT_X11_ERROR_ACCESS] - Access error.
/// - [UT_X11_ERROR_ALLOC] - Alloc error.
/// - [UT_X11_ERROR_COLORMAP] - Colormap error.
/// - [UT_X11_ERROR_GCONTEXT] - Graphics context error.
/// - [UT_X11_ERROR_ID_CHOICE] - ID choice error.
/// - [UT_X11_ERROR_NAME] - Name error.
/// - [UT_X11_ERROR_LENGTH] - Length error.
/// - [UT_X11_ERROR_IMPLEMENTATION] - Implementation error.
/// - [UT_X11_ERROR_BAD_SEGMENT] - Segment error.
/// - [UT_X11_ERROR_INPUT_DEVICE] - Input device error.
/// - [UT_X11_ERROR_INPUT_EVENT] - Input event error.
/// - [UT_X11_ERROR_INPUT_MODE] - Input mode error.
/// - [UT_X11_ERROR_INPUT_DEVICE_BUSY] - Input device busy error.
/// - [UT_X11_ERROR_INPUT_CLASS] - Input class error.
/// - [UT_X11_ERROR_COUNTER] - Counter error.
/// - [UT_X11_ERROR_ALARM] - Alarm error.
/// - [UT_X11_ERROR_BAD_REGION] - Region error.
/// - [UT_X11_ERROR_BAD_OUTPUT] - Output error.
/// - [UT_X11_ERROR_BAD_CRTC] - CRTC error.
/// - [UT_X11_ERROR_BAD_MODE] - Mode error.
/// - [UT_X11_ERROR_BAD_PROVIDER] - Provider error.
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
