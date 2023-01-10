#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1TypeAssignment] object.
///
/// !return-ref
UtObject *ut_asn1_type_assignment_new(const char *reference, UtObject *type);

const char *ut_asn1_type_assignment_get_reference(UtObject *object);

UtObject *ut_asn1_type_assignment_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TypeAssignment].
bool ut_object_is_asn1_type_assignment(UtObject *object);
