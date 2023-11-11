#include <stdbool.h>

#include "ut-object.h"

#pragma once

#define UT_X11_AUTH_FAMILY_LOCAL 256
#define UT_X11_AUTH_FAMILY_WILD 65535

/// Creates a new X11 authorization record.
///
/// !arg-type address UtUint8List
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtX11AuthRecord
UtObject *ut_x11_auth_record_new(uint16_t family, UtObject *address,
                                 const char *display, const char *name,
                                 UtObject *data);

/// Returns the address family this record uses.
uint16_t ut_x11_auth_record_get_family(UtObject *object);

/// Returns the address this record uses.
///
/// !return-type UtUint8List
UtObject *ut_x11_auth_record_get_address(UtObject *object);

/// Returns the display this record is for.
const char *ut_x11_auth_record_get_display(UtObject *object);

/// Returns the name of the authorization scheme.
const char *ut_x11_auth_record_get_name(UtObject *object);

/// Returns the data associated with this authorization scheme.
///
/// !return-type UtUint8List
UtObject *ut_x11_auth_record_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtX11AuthRecord].
bool ut_object_is_x11_auth_record(UtObject *object);
