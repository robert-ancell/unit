#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationPresentationContextId] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_presentation_context_id_new(
    int64_t presentation_context_id);

int64_t ut_asn1_embedded_identification_presentation_context_id_get_value(
    UtObject *object);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationPresentationContextId].
bool ut_object_is_asn1_embedded_identification_presentation_context_id(
    UtObject *object);
