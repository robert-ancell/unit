#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  UtObject *(*get_tags)(UtObject *object);
  bool (*matches_tag)(UtObject *object, UtObject *tag);
  UtObject *(*get_base_type)(UtObject *object);
} UtAsn1TypeInterface;

extern int ut_asn1_type_id;

/// Returns all the tags this type can use.
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_asn1_type_get_tags(UtObject *object);

/// Returns true if this type matches [tag].
///
/// !arg-type tag UtAsn1Tag
bool ut_asn1_type_matches_tag(UtObject *object, UtObject *tag);

/// Checks if [value] is a valid value for this type and returns an error
/// if it is not valid.
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtAsn1Error
UtObject *ut_asn1_type_check_value(UtObject *object, UtObject *value);

/// Returns the lowest level type that this type is derived from.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_type_get_base_type(UtObject *object);

/// Returns [true] if [object] implements [UtAsn1Type].
bool ut_object_implements_asn1_type(UtObject *object);
