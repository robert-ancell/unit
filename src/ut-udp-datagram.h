#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new UDP datagram containing [data] to send to [address]:[port].
///
/// !arg-type address UtIpAddress
/// !arg-type data UtUint8List
/// !return-ref
/// !return-type UtUdpDatagram
UtObject *ut_udp_datagram_new(UtObject *address, uint16_t port, UtObject *data);

/// Returns the address this datagram is for.
///
/// !return-type address UtIpAddress
UtObject *ut_udp_datagram_get_address(UtObject *object);

/// Returns the port this datagram is for.
uint16_t ut_udp_datagram_get_port(UtObject *object);

/// Returns the data in this datagram.
///
/// !return-type data UtUint8List
UtObject *ut_udp_datagram_get_data(UtObject *object);

/// Returns [true] if [object] is a [UtUdpDatagram].
bool ut_object_is_udp_datagram(UtObject *object);
