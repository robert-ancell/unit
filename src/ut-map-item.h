#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  UtObject *(*get_key)(UtObject *object);
  UtObject *(*get_value)(UtObject *object);
} UtMapItemInterface;

extern int ut_map_item_id;

/// Creates a new map item.
///
/// !arg-type key Object
/// !arg-type value Object
/// !return-ref
/// !return-type UtOrderedHashTable
UtObject *ut_map_item_new(UtObject *key, UtObject *value);

/// Gets the key this item is using.
///
/// !return-type UtObject
UtObject *ut_map_item_get_key(UtObject *object);

/// Gets the value this item is using.
///
/// !return-type UtObject
UtObject *ut_map_item_get_value(UtObject *object);

/// Returns [true] if [object] is a [UtMapItem].
bool ut_object_implements_map_item(UtObject *object);
