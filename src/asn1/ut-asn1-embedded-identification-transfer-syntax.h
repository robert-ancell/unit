#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 transfer syntax embedded identification with
/// [identifier].
///
/// !arg-type identifier UtObjectIdentifier
/// !return-ref
/// !return-type UtAsn1EmbeddedIdentificationTransferSyntax
UtObject *
ut_asn1_embedded_identification_transfer_syntax_new(UtObject *identifier);

/// Returns the identifier for this embedded identification.
///
/// !return-type UtObjectIdentifier
UtObject *ut_asn1_embedded_identification_transfer_syntax_get_identifier(
    UtObject *syntax);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationTransferSyntax].
bool ut_object_is_asn1_embedded_identification_transfer_syntax(
    UtObject *object);
