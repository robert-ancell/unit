#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  int16_t (*get_element)(UtObject *object, size_t index);
  const int16_t *(*get_data)(UtObject *object);
  int16_t *(*get_writable_data)(UtObject *object);
  int16_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const int16_t *data,
                 size_t data_length);
} UtInt16ListInterface;

extern int ut_int16_list_id;

UtObject *ut_int16_list_new();

UtObject *ut_int16_list_new_from_elements(size_t length, ...);

int16_t ut_int16_list_get_element(UtObject *object, size_t index);

const int16_t *ut_int16_list_get_data(UtObject *object);

int16_t *ut_int16_list_get_writable_data(UtObject *object);

int16_t *ut_int16_list_take_data(UtObject *object);

void ut_int16_list_append(UtObject *object, int16_t item);

void ut_int16_list_append_block(UtObject *object, const int16_t *data,
                                size_t data_length);

void ut_int16_list_prepend(UtObject *object, int16_t item);

void ut_int16_list_prepend_block(UtObject *object, const int16_t *data,
                                 size_t data_length);

void ut_int16_list_insert(UtObject *object, size_t index, int16_t item);

void ut_int16_list_insert_block(UtObject *object, size_t index,
                                const int16_t *data, size_t data_length);

bool ut_object_implements_int16_list(UtObject *object);
