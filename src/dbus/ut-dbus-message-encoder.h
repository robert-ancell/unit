#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new DBus message encoder.
///
/// !return-ref
/// !return-type UtDbusMessageEncoder
UtObject *ut_dbus_message_encoder_new();

/// Returns [message] encoded.
///
/// !arg-type message UtDbusMessage
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_dbus_message_encoder_encode(UtObject *object, UtObject *message);

/// Returns [true] if [object] is a [UtDbusMessageEncoder].
bool ut_object_is_dbus_message_encoder(UtObject *object);
