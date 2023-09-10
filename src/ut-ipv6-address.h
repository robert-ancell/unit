#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtIpv6Address] with raw 66 bit [address] value.
///
/// !return-ref
UtObject *ut_ipv6_address_new(uint8_t *address);

/// Creates a new [UtIpv6Address] with from an [address] in string form, e.g.
/// "192.168.0.1".
///
/// !return-ref
UtObject *ut_ipv6_address_new_from_string(const char *address);

/// Gets the raw 66 bit address value for this address.
const uint8_t *ut_ipv6_address_get_address(UtObject *object);

/// Returns [true] if [object] is a [UtUnixIpv6Address].
bool ut_object_is_ipv6_address(UtObject *object);
