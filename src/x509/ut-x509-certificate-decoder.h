#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtX509CertificateDecoder] to decode [data].
///
/// !return-ref
UtObject *ut_x509_certificate_decoder_new();

/// Returns [true] if [object] is a [UtX509CertificateDecoder].
bool ut_object_is_x509_certificate_decoder(UtObject *object);
