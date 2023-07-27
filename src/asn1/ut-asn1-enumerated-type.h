#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EnumeratedType] object.
///
/// !return-ref
UtObject *ut_asn1_enumerated_type_new(UtObject *values_by_name,
                                      bool extensible);

int64_t ut_asn1_enumerated_type_get_value(UtObject *object, const char *name);

/// !return-ref
UtObject *ut_asn1_enumerated_type_get_names(UtObject *object);

const char *ut_asn1_enumerated_type_get_name(UtObject *object, int64_t number);

bool ut_asn1_enumerated_type_get_extensible(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EnumeratedType].
bool ut_object_is_asn1_enumerated_type(UtObject *object);
