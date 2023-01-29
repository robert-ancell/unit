#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef struct {
  bool (*get_element)(UtObject *object, size_t index);
  const uint8_t *(*get_data)(UtObject *object);
  uint8_t *(*take_data)(UtObject *object);
  void (*insert)(UtObject *object, size_t index, uint8_t data,
                 size_t bit_count);
  void (*append)(UtObject *object, uint8_t data, size_t bit_count);
} UtBitListInterface;

extern int ut_bit_list_id;

UtObject *ut_bit_list_new();

UtObject *ut_bit_list_new_from_data(const uint8_t *data, size_t bit_count);

UtObject *ut_bit_list_new_from_bin_string(const char *bin);

bool ut_bit_list_get_element(UtObject *object, size_t index);

const uint8_t *ut_bit_list_get_data(UtObject *object);

uint8_t *ut_bit_list_copy_data(UtObject *object);

uint8_t *ut_bit_list_take_data(UtObject *object);

void ut_bit_list_append(UtObject *object, bool value);

void ut_bit_list_append_bits(UtObject *object, uint8_t data, size_t bit_count);

void ut_bit_list_prepend(UtObject *object, bool value);

void ut_bit_list_prepend_bits(UtObject *object, uint8_t data, size_t bit_count);

void ut_bit_list_insert(UtObject *object, size_t index, bool value);

void ut_bit_list_insert_bits(UtObject *object, size_t index, uint8_t data,
                             size_t bit_count);

char *ut_bit_list_to_bin_string(UtObject *object);

bool ut_object_implements_bit_list(UtObject *object);
