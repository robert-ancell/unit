#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 UTCTime type.
///
/// !return-ref
/// !return-type UtAsn1UtcTimeType
UtObject *ut_asn1_utc_time_type_new();

/// Returns [true] if [object] is a [UtAsn1UtcTimeType].
bool ut_object_is_asn1_utc_time_type(UtObject *object);
