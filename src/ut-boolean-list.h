#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  bool (*get_element)(UtObject *object, size_t index);
  void (*insert)(UtObject *object, size_t index, const bool item);
} UtBooleanListInterface;

extern int ut_boolean_list_id;

UtObject *ut_boolean_list_new();

bool ut_boolean_list_get_element(UtObject *object, size_t index);

void ut_boolean_list_append(UtObject *object, bool item);

void ut_boolean_list_prepend(UtObject *object, bool item);

void ut_boolean_list_insert(UtObject *object, size_t index, bool item);

bool ut_object_implements_boolean_list(UtObject *object);
