#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_asn1_error_new(const char *description);

bool ut_object_is_asn1_error(UtObject *object);
