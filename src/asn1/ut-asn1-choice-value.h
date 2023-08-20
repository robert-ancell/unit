#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1ChoiceValue] object.
///
/// !return-ref
UtObject *ut_asn1_choice_value_new(const char *identifier, UtObject *value);

/// Creates a new [UtAsn1ChoiceValue] object.
///
/// !return-ref
UtObject *ut_asn1_choice_value_new_take(const char *identifier,
                                        UtObject *value);

const char *ut_asn1_choice_value_get_identifier(UtObject *object);

UtObject *ut_asn1_choice_value_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1ChoiceValue].
bool ut_object_is_asn1_choice_value(UtObject *object);
