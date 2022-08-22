#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtDnsLookupCallback)(void *user_data, UtObject *address);

UtObject *ut_dns_client_new();

void ut_dns_client_lookup_ipv4(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

void ut_dns_client_lookup_ipv6(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel);

bool ut_object_is_dns_client(UtObject *object);
