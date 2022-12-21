#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// The default port used for DNS.
#define UT_DNS_DEFAULT_PORT 53

/// Function called when a DNS lookup is completed and [address] is resolved.
///
/// !arg-type address UtIpAddress
typedef void (*UtDnsLookupCallback)(void *user_data, UtObject *address);

/// Creates a new [UtDnsClient] to access the DNS server on [server_address] and
/// [port].
///
/// !arg-type UtIpAddress
/// !return-type UtDnsClient
/// !return-ref
UtObject *ut_dns_client_new(UtObject *server_address, uint16_t port);

/// Perform an IPv4 address lookup for the host with [name] using [object].
void ut_dns_client_lookup_ipv4(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

/// Perform an IPv6 address lookup for the host with [name] using [object].
void ut_dns_client_lookup_ipv6(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

/// Returns [true] if [object] is a [UtDnsClient].
bool ut_object_is_dns_client(UtObject *object);
