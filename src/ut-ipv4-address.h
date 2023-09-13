#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtIpv4Address] with raw [address] value.
///
/// !return-ref
/// !return-type UtIpv4Address
UtObject *ut_ipv4_address_new(uint32_t address);

/// Creates a new IPv4 address with value [a0].[a1].[a2].[a3], e.g.
/// 192.168.0.1.
///
/// !return-ref
/// !return-type UtIpv4Address
UtObject *ut_ipv4_address_new_from_quad(uint8_t a0, uint8_t a1, uint8_t a2,
                                        uint8_t a3);

/// Creates a new IPv4 address from an [address] in string form, e.g.
/// "192.168.0.1".
///
/// !return-ref
/// !return-type UtIpv4Address
UtObject *ut_ipv4_address_new_from_string(const char *address);

/// Creates a new IPv4 address for the loopback address 127.0.0.1.
///
/// !return-ref
/// !return-type UtIpv4Address
UtObject *ut_ipv4_address_new_loopback();

/// Gets the raw address value for this address.
uint32_t ut_ipv4_address_get_address(UtObject *object);

/// Returns [true] if [object] is a [UtUnixIpv4Address].
bool ut_object_is_ipv4_address(UtObject *object);
