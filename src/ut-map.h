#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "ut-object.h"

#pragma once

typedef struct {
  size_t (*get_length)(UtObject *object);
  void (*insert)(UtObject *object, UtObject *key, UtObject *value);
  UtObject *(*lookup)(UtObject *object, UtObject *key);
  void (*remove)(UtObject *object, UtObject *key);
  UtObject *(*get_items)(UtObject *object);
  UtObject *(*get_keys)(UtObject *object);
  UtObject *(*get_values)(UtObject *object);
} UtMapInterface;

extern int ut_map_id;

/// Creates a new ordered map.
///
/// !return-ref
/// !return-type UtMap
UtObject *ut_map_new();

/// Creates a new unordered map.
///
/// !return-ref
/// !return-type UtMap
UtObject *ut_map_new_unordered();

/// Creates a new map containing the provided [key] [value] pairs.
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtMap
UtObject *ut_map_new_string_from_elements(const char *key, UtObject *value,
                                          ...);

/// Creates a new map containing the provided [key] [value] pairs.
/// The values must pass in references.
///
/// !arg-type value UtObject
/// !return-ref
/// !return-type UtMap
UtObject *ut_map_new_string_from_elements_take(const char *key, UtObject *value,
                                               ...);

/// Returns the number of key/value pairs in this map.
size_t ut_map_get_length(UtObject *object);

/// Insert [key]/[value] into the map.
/// If an existing entry containins this key it will be replaced.
///
/// !arg-type key UtObject
/// !arg-type value UtObject
void ut_map_insert(UtObject *object, UtObject *key, UtObject *value);

/// Insert [key]/[value] into the map.
/// If an existing entry containins this key it will be replaced.
/// Both [key] and [value] must pass in references.
///
/// !arg-type key UtObject
/// !arg-type value UtObject
void ut_map_insert_take(UtObject *object, UtObject *key, UtObject *value);

/// Insert [item] into the map.
/// If an existing entry containins this key it will be replaced.
///
/// !arg-type item UtMapItem
void ut_map_insert_item(UtObject *object, UtObject *item);

/// Insert [key]/[value] into the map.
/// If an existing entry containins this key it will be replaced.
///
/// !arg-type value UtObject
void ut_map_insert_string(UtObject *object, const char *key, UtObject *value);

/// Insert [key]/[value] into the map.
/// If an existing entry containins this key it will be replaced.
/// [value] must pass in a reference.
///
/// !arg-type value UtObject
void ut_map_insert_string_take(UtObject *object, const char *key,
                               UtObject *value);

/// Get the value referenced by [key] in this map, or [NULL] if no element with
/// that key.
///
/// !arg-type key UtObject
/// !return-type UtObject NULL
UtObject *ut_map_lookup(UtObject *object, UtObject *key);

/// Get the value referenced by [key] in this map, or [NULL] if no element with
/// that key.
///
/// !return-type UtObject NULL
UtObject *ut_map_lookup_string(UtObject *object, const char *key);

/// Remove the element in this map that has [key].
/// If no element has this key, no action is performed.
///
/// !arg-type key UtObject
void ut_map_remove(UtObject *object, UtObject *key);

/// Get all the elements in this map.
/// If the map is unordered, the elements are returned in an undefined order.
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_map_get_items(UtObject *object);

/// Get all the keys in this map.
/// If the map is unordered, the keys are returned in an undefined order.
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_map_get_keys(UtObject *object);

/// Get all the values in this map.
/// If the map is unordered, the values are returned in an undefined order.
///
/// !return-ref
/// !return-type UtObjectList
UtObject *ut_map_get_values(UtObject *object);

/// Returns [true] if [object] is a [UtMap].
bool ut_object_implements_map(UtObject *object);
