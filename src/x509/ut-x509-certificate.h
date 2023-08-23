#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtX509Cerfificate].
///
/// !return-ref
UtObject *ut_x509_certificate_new();

/// Returns [true] if [object] is a [UtX509Cerfificate].
bool ut_object_is_x509_certificate(UtObject *object);
