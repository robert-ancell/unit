#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedPdvType] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_pdv_type_new();

/// !return-ref
UtObject *ut_asn1_embedded_pdv_type_get_associated_type(UtObject *object);

/// Encodes [value] into a form suitable for encoding with the associated type.
/// !return-ref
UtObject *ut_asn1_embedded_pdv_type_encode_value(UtObject *object,
                                                 UtObject *value);

/// Decodes [value] decoded from the associated type into an
/// [UtAsn1EmbeddedPdv]. !return-ref
UtObject *ut_asn1_embedded_pdv_type_decode_value(UtObject *object,
                                                 UtObject *value);

/// Returns [true] if [object] is a [UtAsn1EmbeddedPdvType].
bool ut_object_is_asn1_embedded_pdv_type(UtObject *object);
