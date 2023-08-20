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

/// Ordered by default.
/// !return-ref
UtObject *ut_map_new();

/// !return-ref
UtObject *ut_map_new_unordered();

/// !return-ref
UtObject *ut_map_new_string_from_elements_take(const char *key, UtObject *value,
                                               ...);

size_t ut_map_get_length(UtObject *object);

void ut_map_insert(UtObject *object, UtObject *key, UtObject *value);

void ut_map_insert_take(UtObject *object, UtObject *key, UtObject *value);

void ut_map_insert_string(UtObject *object, const char *key, UtObject *value);

void ut_map_insert_string_take(UtObject *object, const char *key,
                               UtObject *value);

UtObject *ut_map_lookup(UtObject *object, UtObject *key);

UtObject *ut_map_lookup_string(UtObject *object, const char *key);

void ut_map_remove(UtObject *object, UtObject *key);

/// !return-ref
UtObject *ut_map_get_items(UtObject *object);

/// !return-ref
UtObject *ut_map_get_keys(UtObject *object);

/// !return-ref
UtObject *ut_map_get_values(UtObject *object);

char *ut_map_to_string(UtObject *object);

bool ut_object_implements_map(UtObject *object);
