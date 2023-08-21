#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1DefaultType] object.
///
/// !return-ref
UtObject *ut_asn1_default_type_new(UtObject *type, UtObject *default_value);

/// Creates a new [UtAsn1DefaultType] object.
///
/// !return-ref
UtObject *ut_asn1_default_type_new_take(UtObject *type,
                                        UtObject *default_value);

UtObject *ut_asn1_default_type_get_type(UtObject *object);

UtObject *ut_asn1_default_type_get_default_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1DefaultType].
bool ut_object_is_asn1_default_type(UtObject *object);
