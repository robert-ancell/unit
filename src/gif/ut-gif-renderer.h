#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_gif_renderer_new(UtObject *decoder);

UtObject *ut_gif_renderer_render(UtObject *object);

bool ut_object_is_gif_renderer(UtObject *object);