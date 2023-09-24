#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 embedded value with [data_value] identified by
/// [identification] and optional [data_value_descriptor].
///
/// !arg-type identification UtAsn1EmbeddedIdentificationSyntaxes
/// !arg-type identification UtAsn1EmbeddedIdentificationSyntax
/// !arg-type identification UtAsn1EmbeddedIdentificationPresentationContextId
/// !arg-type identification UtAsn1EmbeddedIdentificationContextNegotiation
/// !arg-type identification UtAsn1EmbeddedIdentificationTransferSyntax
/// !arg-type identification UtAsn1EmbeddedIdentificationFixed
/// !arg-type data_value UtUint8List
/// !return-ref
/// !return-type UtAsn1EmbeddedValue
UtObject *ut_asn1_embedded_value_new(UtObject *identification,
                                     const char *data_value_descriptor,
                                     UtObject *data_value);

/// Creates a new ASN.1 embedded value with [data_value] identified by
/// [identification] and optional [data_value_descriptor].
///
/// !arg-type identification UtAsn1EmbeddedIdentificationSyntaxes
/// !arg-type identification UtAsn1EmbeddedIdentificationSyntax
/// !arg-type identification UtAsn1EmbeddedIdentificationPresentationContextId
/// !arg-type identification UtAsn1EmbeddedIdentificationContextNegotiation
/// !arg-type identification UtAsn1EmbeddedIdentificationTransferSyntax
/// !arg-type identification UtAsn1EmbeddedIdentificationFixed
/// !arg-type data_value UtUint8List
/// !return-ref
/// !return-type UtAsn1EmbeddedValue
UtObject *ut_asn1_embedded_value_new_take(UtObject *identification,
                                          const char *data_value_descriptor,
                                          UtObject *data_value);

/// Returns the identification for this embedded value.
///
/// !return-type identification UtAsn1EmbeddedIdentificationSyntaxes
/// UtAsn1EmbeddedIdentificationSyntax
/// UtAsn1EmbeddedIdentificationPresentationContextId
/// UtAsn1EmbeddedIdentificationContextNegotiation
/// UtAsn1EmbeddedIdentificationTransferSyntax UtAsn1EmbeddedIdentificationFixed
UtObject *ut_asn1_embedded_value_get_identification(UtObject *object);

/// Returns the data value descriptor for this embedded value or [NULL] if none
/// set.
const char *ut_asn1_embedded_value_get_data_value_descriptor(UtObject *object);

/// Returns the data for this embedded value.
///
/// !return-type UtUint8List
UtObject *ut_asn1_embedded_value_get_data_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1EmbeddedValue].
bool ut_object_is_asn1_embedded_value(UtObject *object);
