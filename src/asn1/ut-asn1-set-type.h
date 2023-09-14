#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1SetType] object.
///
/// !return-ref
UtObject *ut_asn1_set_type_new(UtObject *components, bool extensible);

/// Creates a new [UtAsn1SetType] object.
///
/// !return-ref
UtObject *ut_asn1_set_type_new_take(UtObject *components, bool extensible);

UtObject *ut_asn1_set_type_get_components(UtObject *object);

/// Returns true if this type is extensible.
bool ut_asn1_set_type_get_extensible(UtObject *object);

UtObject *ut_asn1_set_type_get_component(UtObject *object, const char *name);

/// Returns [true] if [object] is a [UtAsn1SetType].
bool ut_object_is_asn1_set_type(UtObject *object);
