#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtIPAddressResolverLookupCallback)(UtObject *object,
                                                  UtObject *addresses);

UtObject *ut_ip_address_resolver_new();

void ut_ip_address_resolver_lookup(UtObject *object, const char *name,
                                   UtObject *callback_object,
                                   UtIPAddressResolverLookupCallback callback);

bool ut_object_is_ip_address_resolver(UtObject *object);
