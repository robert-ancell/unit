#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_ipv6_address_new(uint8_t *address);

UtObject *ut_ipv6_address_new_from_string(const char *address);

const uint8_t *ut_ipv6_address_get_address(UtObject *object);

bool ut_object_is_ipv6_address(UtObject *object);
