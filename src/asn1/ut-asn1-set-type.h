#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 SET type.
/// [components] contains a map of set component names to ASN.1 types.
/// If [extensible] is [true] then then encoded forms of this set may contain
/// values not in [components].
///
/// !arg-type components UtMap
/// !return-ref
/// !return-type UtAsn1SetType
UtObject *ut_asn1_set_type_new(UtObject *components, bool extensible);

/// Creates a new ASN.1 SET type.
/// [components] contains a map of set component names to ASN.1 types.
/// If [extensible] is [true] then then encoded forms of this set may contain
/// values not in [components].
///
/// !arg-type components UtMap
/// !return-ref
/// !return-type UtAsn1SetType
UtObject *ut_asn1_set_type_new_take(UtObject *components, bool extensible);

/// Returns the components in this set.
///
/// !return-type UtMap
UtObject *ut_asn1_set_type_get_components(UtObject *object);

/// Returns true if this type is extensible.
bool ut_asn1_set_type_get_extensible(UtObject *object);

/// Returns the type of the component with [name] or [NULL] if no component of
/// that name.
///
/// !return-type UtObject NULL
UtObject *ut_asn1_set_type_lookup_component(UtObject *object, const char *name);

/// Returns [true] if [object] is a [UtAsn1SetType].
bool ut_object_is_asn1_set_type(UtObject *object);
