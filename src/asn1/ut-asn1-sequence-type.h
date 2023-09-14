#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 SEQUENCE type.
///
/// !return-ref
/// !return-type UtAsn1SequenceType
UtObject *ut_asn1_sequence_type_new(UtObject *components, bool extensible);

/// Creates a new ASN.1 SEQUENCE type.
///
/// !return-ref
/// !return-type UtAsn1SequenceType
UtObject *ut_asn1_sequence_type_new_take(UtObject *components, bool extensible);

/// Returns the components in this sequence.
///
/// !return-type UtObjectList
UtObject *ut_asn1_sequence_type_get_components(UtObject *object);

/// Returns true if this type is extensible.
bool ut_asn1_sequence_type_get_extensible(UtObject *object);

/// Returns the type of the component with [name] or [NULL] if no component of
/// that name.
///
/// !return-type UtObject NULL
UtObject *ut_asn1_sequence_type_lookup_component(UtObject *object,
                                                 const char *name);

/// Returns [true] if [object] is a [UtAsn1SequenceType].
bool ut_object_is_asn1_sequence_type(UtObject *object);
