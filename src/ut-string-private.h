#include "ut-object.h"

#pragma once

char *_ut_string_to_string(UtObject *object);

bool _ut_string_equal(UtObject *object, UtObject *other);

int _ut_string_hash(UtObject *object);
