#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_udp_datagram_new(UtObject *address, uint16_t port, UtObject *data);

UtObject *ut_udp_datagram_get_address(UtObject *object);

uint16_t ut_udp_datagram_get_port(UtObject *object);

UtObject *ut_udp_datagram_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtUdpDatagram].
bool ut_object_is_udp_datagram(UtObject *object);
