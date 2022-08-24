#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_ipv4_address_new(uint32_t address);

UtObject *ut_ipv4_address_new_from_quad(uint8_t a0, uint8_t a1, uint8_t a2,
                                        uint8_t a3);

UtObject *ut_ipv4_address_new_from_string(const char *address);

UtObject *ut_ipv4_address_new_loopback();

uint32_t ut_ipv4_address_get_address(UtObject *object);

bool ut_object_is_ipv4_address(UtObject *object);
