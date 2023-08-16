#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  UtObject *(*get_tags)(UtObject *object);
} UtAsn1TypeInterface;

extern int ut_asn1_type_id;

UtObject *ut_asn1_type_get_tags(UtObject *object);

/// Returns [true] if [object] implements [UtAsn1Type].
bool ut_object_implements_asn1_type(UtObject *object);
