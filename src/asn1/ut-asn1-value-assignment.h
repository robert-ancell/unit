#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ValueAssignment] object.
///
/// !return-ref
UtObject *ut_asn1_value_assignment_new(const char *reference, UtObject *type,
                                       UtObject *value);

const char *ut_asn1_value_assignment_get_reference(UtObject *object);

UtObject *ut_asn1_value_assignment_get_value(UtObject *object);

UtObject *ut_asn1_value_assignment_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ValueAssignment].
bool ut_object_is_asn1_value_assignment(UtObject *object);
