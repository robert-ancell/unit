#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 embedded value.
///
/// !arg-type identification UtObjectIdentifier
/// !arg-type data_value UtUint8List
/// !return-ref
/// !return-type UtAsn1EmbeddedValue
UtObject *ut_asn1_embedded_value_new(UtObject *identification,
                                     const char *data_value_descriptor,
                                     UtObject *data_value);

/// Creates a new ASN.1 embedded value.
///
/// !arg-type identification UtObjectIdentifier
/// !arg-type data_value UtUint8List
/// !return-ref
/// !return-type UtAsn1EmbeddedValue
UtObject *ut_asn1_embedded_value_new_take(UtObject *identification,
                                          const char *data_value_descriptor,
                                          UtObject *data_value);

/// Returns the identification for this embedded value.
///
/// !return-type UtObjectIdentifier
UtObject *ut_asn1_embedded_value_get_identification(UtObject *object);

/// Returns the data value descriptor for this embedded value.
const char *ut_asn1_embedded_value_get_data_value_descriptor(UtObject *object);

/// Returns the data for this embedded value.
///
/// !return-type UtUint8List
UtObject *ut_asn1_embedded_value_get_data_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EmbeddedValue].
bool ut_object_is_asn1_embedded_value(UtObject *object);
