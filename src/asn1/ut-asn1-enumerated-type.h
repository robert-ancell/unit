#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 ENUMERATED type with [values_by_name].
///
/// !return-ref
/// !return-type UtAsn1EnumeratedType
UtObject *ut_asn1_enumerated_type_new(UtObject *values_by_name,
                                      bool extensible);

/// Returns the enumerate value with [name] or -1 if none found.
int64_t ut_asn1_enumerated_type_lookup_value(UtObject *object,
                                             const char *name);

/// Returns the name for enumerated value [number] or [NULL] if none found.
const char *ut_asn1_enumerated_type_lookup_name(UtObject *object,
                                                int64_t number);

/// Returns true if this type is extensible.
bool ut_asn1_enumerated_type_get_extensible(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EnumeratedType].
bool ut_object_is_asn1_enumerated_type(UtObject *object);
