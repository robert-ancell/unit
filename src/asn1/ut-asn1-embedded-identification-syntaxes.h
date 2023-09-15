#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 syntaxes embedded identification with [abstract] and
/// [transfer] identifiers.
///
/// !arg-type abstract UtObjectIdentifier
/// !arg-type transfer UtObjectIdentifier
/// !return-ref
/// !return-type UtAsn1EmbeddedIdentificationSyntaxes
UtObject *ut_asn1_embedded_identification_syntaxes_new(UtObject *abstract,
                                                       UtObject *transfer);

/// Returns the abstract identifier for this embedded identification.
///
/// !return-type UtObjectIdentifier
UtObject *
ut_asn1_embedded_identification_syntaxes_get_abstract(UtObject *object);

/// Returns the transfer identifier for this embedded identification.
///
/// !return-type UtObjectIdentifier
UtObject *
ut_asn1_embedded_identification_syntaxes_get_transfer(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EmbeddedIdentificationSyntaxes].
bool ut_object_is_asn1_embedded_identification_syntaxes(UtObject *object);
