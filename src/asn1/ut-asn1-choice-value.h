#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 CHOICE value with [identifier] and [value].
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtAsn1ChoiceValue
UtObject *ut_asn1_choice_value_new(const char *identifier, UtObject *value);

/// Creates a new ASN.1 CHOICE value with [identifier] and [value].
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtAsn1ChoiceValue
UtObject *ut_asn1_choice_value_new_take(const char *identifier,
                                        UtObject *value);

/// Returns the identifier for this choice value.
const char *ut_asn1_choice_value_get_identifier(UtObject *object);

/// Returns the value for this choice value.
///
/// !return-type UtObject
UtObject *ut_asn1_choice_value_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ChoiceValue].
bool ut_object_is_asn1_choice_value(UtObject *object);
