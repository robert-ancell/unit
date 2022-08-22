#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

#define UT_DNS_DEFAULT_PORT 53

typedef void (*UtDnsLookupCallback)(void *user_data, UtObject *address);

UtObject *ut_dns_client_new(UtObject *server_address, uint16_t port);

void ut_dns_client_lookup_ipv4(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

void ut_dns_client_lookup_ipv6(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

bool ut_object_is_dns_client(UtObject *object);
