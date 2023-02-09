#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_deflate_encoder_new(UtObject *input_stream);

UtObject *ut_deflate_encoder_new_with_window_size(size_t window_size,
                                                  UtObject *input_stream);

size_t ut_deflate_encoder_get_window_size(UtObject *object);

bool ut_object_is_deflate_encoder(UtObject *object);
