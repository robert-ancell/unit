#include "ut-object.h"

#pragma once

/// Encodes an object that implements [UtUint8List] into a base64 encoding.
char *ut_base64_encode(UtObject *object);

/// Decodes base64 text into a [UtUint8List] object.
UtObject *ut_base64_decode(const char *text);
