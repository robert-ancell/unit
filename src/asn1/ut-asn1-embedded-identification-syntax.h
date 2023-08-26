#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedIdentificationSyntax] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_identification_syntax_new(UtObject *value);

UtObject *ut_asn1_embedded_identification_syntax_get_value(UtObject *syntax);

/// Returns [true] if [object] is a [UtAsn1EmbeddedIdentificationSyntax].
bool ut_object_is_asn1_embedded_identification_syntax(UtObject *object);
