#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzma2_decoder_new(UtObject *input_stream);

bool ut_object_is_lzma2_decoder(UtObject *object);
