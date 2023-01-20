#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_deflate_encoder_new(UtObject *input_stream);

bool ut_object_is_deflate_encoder(UtObject *object);
