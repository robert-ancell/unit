#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationSyntaxes] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_syntaxes_new(UtObject *abstract,
                                                       UtObject *transfer);

UtObject *
ut_asn1_embedded_identification_syntaxes_get_abstract(UtObject *object);

UtObject *
ut_asn1_embedded_identification_syntaxes_get_transfer(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EmbeddedIdentificationSyntaxes].
bool ut_object_is_asn1_embedded_identification_syntaxes(UtObject *object);
