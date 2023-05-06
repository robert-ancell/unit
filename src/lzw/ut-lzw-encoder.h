#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_lzw_encoder_new_lsb(UtObject *input_stream);

UtObject *ut_lzw_encoder_new_msb(UtObject *input_stream);

bool ut_object_is_lzw_encoder(UtObject *object);
