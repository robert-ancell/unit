#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzw_decoder_new_lsb(size_t n_symbols, UtObject *input_stream);

UtObject *ut_lzw_decoder_new_msb(size_t n_symbols, UtObject *input_stream);

bool ut_object_is_lzw_decoder(UtObject *object);
