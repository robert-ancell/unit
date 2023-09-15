#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 [presentation_context_id] embedded identification.
///
/// !return-ref
/// !return-type UtAsn1EmbeddedIdentificationPresentationContextId
UtObject *ut_asn1_embedded_identification_presentation_context_id_new(
    int64_t presentation_context_id);

/// Returns the presentation context ID for this embedded identification.
int64_t ut_asn1_embedded_identification_presentation_context_id_get_value(
    UtObject *object);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationPresentationContextId].
bool ut_object_is_asn1_embedded_identification_presentation_context_id(
    UtObject *object);
