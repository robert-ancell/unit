#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef struct {
  char *(*to_string)(UtObject *object);
} UtIPAddressInterface;

extern int ut_ip_address_id;

/// Returns this IP address in string form, e.g. "192.168.0.1"
char *ut_ip_address_to_string(UtObject *object);

/// Returns [true] if [object] is a [UtIpAddress].
bool ut_object_implements_ip_address(UtObject *object);
