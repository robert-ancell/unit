#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 context negotiation embedded identification with
/// [presentation_context_id] and [transfer_syntax].
///
/// !arg-type transfer_syntax UtObjectIdentifier
/// !return-ref
/// !return-type UtAsn1EmbeddedIdentificationContextNegotiation
UtObject *ut_asn1_embedded_identification_context_negotiation_new(
    int64_t presentation_context_id, UtObject *transfer_syntax);

/// Returns the presentation context ID for this embedded identification.
int64_t
ut_asn1_embedded_identification_context_negotiation_get_presentation_context_id(
    UtObject *object);

/// Returns the transfer syntax for this embedded identification.
///
/// !return-type UtObjectIdentifier
UtObject *
ut_asn1_embedded_identification_context_negotiation_get_transfer_syntax(
    UtObject *object);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationContextNegotiation].
bool ut_object_is_asn1_embedded_identification_context_negotiation(
    UtObject *object);
