#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 EMBEDDED PDV object.
///
/// !return-ref
/// !return-type UtAsn1EmbeddedPdvType
UtObject *ut_asn1_embedded_pdv_type_new();

/// Returns a sequence type that the EMBEDDED PDV type is encoded with.
///
/// !return-ref
/// !return-type UtAsn1Type
UtObject *ut_asn1_embedded_pdv_type_get_associated_type(UtObject *object);

/// Encodes [value] into a form suitable for encoding with the associated type.
///
/// !arg-type value UtAsn1EmbeddedValue
/// !return-ref
/// !return-type UtMap
UtObject *ut_asn1_embedded_pdv_type_encode_value(UtObject *object,
                                                 UtObject *value);

/// Decodes [value] decoded from the associated type into a value.
///
/// !arg-type value UtMap
/// !return-ref
/// !return-type UtAsn1EmbeddedValue
UtObject *ut_asn1_embedded_pdv_type_decode_value(UtObject *object,
                                                 UtObject *value);

/// Returns [true] if [object] is a [UtAsn1EmbeddedPdvType].
bool ut_object_is_asn1_embedded_pdv_type(UtObject *object);
