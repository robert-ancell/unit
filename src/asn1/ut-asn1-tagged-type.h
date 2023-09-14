#include <stdbool.h>

#include "ut-asn1-tag.h"
#include "ut-object.h"

#pragma once

/// Creates a new ASN.1 tagged type.
///
/// !return-ref
/// !return-type UtAsn1TaggedType
UtObject *ut_asn1_tagged_type_new(UtAsn1TagClass class, unsigned int number,
                                  bool is_explicit, UtObject *type);

/// Creates a new ASN.1 tagged type.
///
/// !return-ref
/// !return-type UtAsn1TaggedType
UtObject *ut_asn1_tagged_type_new_take(UtAsn1TagClass class,
                                       unsigned int number, bool is_explicit,
                                       UtObject *type);

/// Returns the class this type is tagged with.
UtAsn1TagClass ut_asn1_tagged_type_get_class(UtObject *object);

/// Returns the tag number this type is tagged with.
unsigned int ut_asn1_tagged_type_get_number(UtObject *object);

/// Returns true if the tagged type will also be encoded with its own tag.
bool ut_asn1_tagged_type_get_is_explicit(UtObject *object);

/// Returns the type that is being tagged.
///
/// !return-type UtAsn1Type
UtObject *ut_asn1_tagged_type_get_type(UtObject *object);

/// Returns [true] if [object] is a [UtAsn1TaggedType].
bool ut_object_is_asn1_tagged_type(UtObject *object);
