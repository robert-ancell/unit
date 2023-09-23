#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// The default port used for DNS.
#define UT_DNS_DEFAULT_PORT 53

/// Function called when a DNS lookup is completed and [address] is resolved.
///
/// !arg-type address UtIpAddress
typedef void (*UtDnsLookupCallback)(UtObject *object, UtObject *address);

/// Creates a new DNS client to access the DNS server on [server_address] and
/// [port].
///
/// !arg-type server_address UtIpAddress
/// !return-type UtDnsClient
/// !return-ref
UtObject *ut_dns_client_new(UtObject *server_address, uint16_t port);

/// Perform an IPv4 address lookup for the host with [name].
/// The responses are returned in [callback].
void ut_dns_client_lookup_ipv4(UtObject *object, const char *name,
                               UtObject *callback_object,
                               UtDnsLookupCallback callback);

/// Perform an IPv6 address lookup for the host with [name].
/// The responses are returned in [callback].
void ut_dns_client_lookup_ipv6(UtObject *object, const char *name,
                               UtObject *callback_object,
                               UtDnsLookupCallback callback);

/// Returns [true] if [object] is a [UtDnsClient].
bool ut_object_is_dns_client(UtObject *object);
