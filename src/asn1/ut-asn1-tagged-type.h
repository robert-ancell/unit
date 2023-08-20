#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new [UtAsn1TaggedType] object.
///
/// !return-ref
UtObject *ut_asn1_tagged_type_new(UtAsn1TagClass class, unsigned int number,
                                  bool is_explicit, UtObject *type);

/// Creates a new [UtAsn1TaggedType] object.
///
/// !return-ref
UtObject *ut_asn1_tagged_type_new_take(UtAsn1TagClass class,
                                       unsigned int number, bool is_explicit,
                                       UtObject *type);

UtAsn1TagClass ut_asn1_tagged_type_get_class(UtObject *object);

unsigned int ut_asn1_tagged_type_get_number(UtObject *object);

bool ut_asn1_tagged_type_get_is_explicit(UtObject *object);

UtObject *ut_asn1_tagged_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TaggedType].
bool ut_object_is_asn1_tagged_type(UtObject *object);
