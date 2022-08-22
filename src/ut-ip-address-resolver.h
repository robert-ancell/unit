#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtIPAddressResolverLookupCallback)(void *user_data,
                                                  UtObject *addresses);

UtObject *ut_ip_address_resolver_new();

void ut_ip_address_resolver_lookup(UtObject *object, const char *name,
                                   UtIPAddressResolverLookupCallback callback,
                                   void *user_data, UtObject *cancel);

bool ut_object_is_ip_address_resolver(UtObject *object);
