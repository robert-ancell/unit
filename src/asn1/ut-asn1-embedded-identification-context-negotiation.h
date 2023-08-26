#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationContextNegotiation] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_context_negotiation_new(
    int64_t presentation_context_id, UtObject *transfer_syntax);

int64_t
ut_asn1_embedded_identification_context_negotiation_get_presentation_context_id(
    UtObject *object);

UtObject *
ut_asn1_embedded_identification_context_negotiation_get_transfer_syntax(
    UtObject *object);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationContextNegotiation].
bool ut_object_is_asn1_embedded_identification_context_negotiation(
    UtObject *object);
