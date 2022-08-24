#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_udp_socket_new_ipv4();

UtObject *ut_udp_socket_new_ipv6();

void ut_udp_socket_bind(UtObject *object, uint16_t port);

uint16_t ut_udp_socket_get_port(UtObject *object);

bool ut_object_is_udp_socket(UtObject *object);
