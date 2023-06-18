#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

// Create an LZW dictionary.
UtObject *ut_lzw_dictionary_new(size_t n_symbols, size_t max_length);

// Get the code used to clear the dictionary.
uint16_t ut_lzw_dictionary_get_clear_code(UtObject *object);

// Get the code used to mark the end of the data.
uint16_t ut_lzw_dictionary_get_end_of_information_code(UtObject *object);

// Get the number of codes in this dictionary.
size_t ut_lzw_dictionary_get_length(UtObject *object);

// Returns true if the dictionary is full.
bool ut_lzw_dictionary_get_is_full(UtObject *object);

// Get the symbols for a given code.
UtObject *ut_lzw_dictionary_lookup(UtObject *object, uint16_t code);

// Reset the dictionary to the initial state.
void ut_lzw_dictionary_clear(UtObject *object);

// Add a new entry to the dictionary.
void ut_lzw_dictionary_append(UtObject *object, uint16_t code, uint8_t b);

bool ut_object_is_lzw_dictionary(UtObject *object);
