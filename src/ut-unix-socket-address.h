#include <stdbool.h>

#include "ut-object.h"

#pragma once

UtObject *ut_unix_socket_address_new(const char *path);

const char *ut_unix_socket_address_get_path(UtObject *object);

bool ut_object_is_unix_socket_address(UtObject *object);
