#include "ut-object.h"

#pragma once

/// Encodes an 8 bit list into a base64 encoding.
///
/// !arg-type object UtUint8List
char *ut_base64_encode(UtObject *object);

/// Decodes base64 [text] into an 8 bit list.
///
/// !return-ref
/// !return-type UtUint8List
UtObject *ut_base64_decode(const char *text);
