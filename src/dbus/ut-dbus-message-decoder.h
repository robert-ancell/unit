#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus message decoder to decoder messages from [input_stream].
///
/// !arg-type input_stream UtDbusMessage
/// !return-ref
/// !return-type UtDbusMessageDecoder
UtObject *ut_dbus_message_decoder_new(UtObject *input_stream);

/// Returns [true] if [object] is a [UtDbusMessageDecoder].
bool ut_object_is_dbus_message_decoder(UtObject *object);
