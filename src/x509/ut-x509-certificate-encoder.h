#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtX509CertificateEncoder].
///
/// !return-ref
UtObject *ut_x509_certificate_encoder_new();

/// Returns [true] if [object] is a [UtX509CertificateEncoder].
bool ut_object_is_x509_certificate_encoder(UtObject *object);
