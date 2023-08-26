#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationTransferSyntax] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_transfer_syntax_new(UtObject *value);

UtObject *
ut_asn1_embedded_identification_transfer_syntax_get_value(UtObject *syntax);

/// Returns [true] if [object] is a
/// [UtAsn1EmbeddedIdentificationTransferSyntax].
bool ut_object_is_asn1_embedded_identification_transfer_syntax(
    UtObject *object);
