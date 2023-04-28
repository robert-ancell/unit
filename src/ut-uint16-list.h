#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  uint16_t (*get_element)(UtObject *object, size_t index);
  const uint16_t *(*get_data)(UtObject *object);
  uint16_t *(*get_writable_data)(UtObject *object);
  uint16_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, const uint16_t *data,
                 size_t data_length);
} UtUint16ListInterface;

extern int ut_uint16_list_id;

UtObject *ut_uint16_list_new();

UtObject *ut_uint16_list_new_from_elements(size_t length, ...);

UtObject *ut_uint16_list_new_from_hex_string(const char *hex);

uint16_t ut_uint16_list_get_element(UtObject *object, size_t index);

const uint16_t *ut_uint16_list_get_data(UtObject *object);

uint16_t *ut_uint16_list_get_writable_data(UtObject *object);

uint16_t *ut_uint16_list_take_data(UtObject *object);

void ut_uint16_list_append(UtObject *object, uint16_t item);

void ut_uint16_list_append_block(UtObject *object, const uint16_t *data,
                                 size_t data_length);

void ut_uint16_list_prepend(UtObject *object, uint16_t item);

void ut_uint16_list_prepend_block(UtObject *object, const uint16_t *data,
                                  size_t data_length);

void ut_uint16_list_insert(UtObject *object, size_t index, uint16_t item);

void ut_uint16_list_insert_block(UtObject *object, size_t index,
                                 const uint16_t *data, size_t data_length);

char *ut_uint16_list_to_hex_string(UtObject *object);

bool ut_object_implements_uint16_list(UtObject *object);
