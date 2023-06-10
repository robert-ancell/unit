#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

// Codes 0-255 are literal bytes.

// Predefined clear code.
#define LZW_CLEAR_CODE 256

// Predefined end of information code.
#define LZW_END_OF_INFORMATION_CODE 257

// Create an LZW dictionary.
UtObject *ut_lzw_dictionary_new();

// Get the number of codes in this dictionary.
size_t ut_lzw_dictionary_get_length(UtObject *object);

// Get the symbols for a given code.
UtObject *ut_lzw_dictionary_lookup(UtObject *object, uint16_t code);

// Get the number of bits required to send a code in the dictionary.
size_t ut_lzw_dictionary_get_code_length(UtObject *object);

// Reset the dictionary to the initial state.
void ut_lzw_dictionary_clear(UtObject *object);

// Add a new entry to the dictionary.
void ut_lzw_dictionary_append(UtObject *object, uint16_t code, uint8_t b);

bool ut_object_is_lzw_dictionary(UtObject *object);
