#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 syntax embedded identification with [identifier].
///
/// !arg-type identifier UtObjectIdentifier
/// !return-ref
/// !return-type UtAsn1EmbeddedIdentificationSyntax
UtObject *ut_asn1_embedded_identification_syntax_new(UtObject *identifier);

/// Returns the identifier for this embedded identification.
///
/// !return-type UtObjectIdentifier
UtObject *
ut_asn1_embedded_identification_syntax_get_identifier(UtObject *syntax);

/// Returns [true] if [object] is a [UtAsn1EmbeddedIdentificationSyntax].
bool ut_object_is_asn1_embedded_identification_syntax(UtObject *object);
