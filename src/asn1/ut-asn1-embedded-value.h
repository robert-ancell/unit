#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1EmbeddedValue] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_value_new(UtObject *identification,
                                     const char *data_value_descriptor,
                                     UtObject *data_value);

/// Creates a new [UtAsn1EmbeddedValue] object.
///
/// !return-ref
UtObject *ut_asn1_embedded_value_new_take(UtObject *identification,
                                          const char *data_value_descriptor,
                                          UtObject *data_value);

UtObject *ut_asn1_embedded_value_get_identification(UtObject *object);

const char *ut_asn1_embedded_value_get_data_value_descriptor(UtObject *object);

UtObject *ut_asn1_embedded_value_get_data_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EmbeddedValue].
bool ut_object_is_asn1_embedded_value(UtObject *object);
