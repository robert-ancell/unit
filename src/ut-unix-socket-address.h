#include <stdbool.h>

#include "ut-object.h"

#pragma once

/// Creates a new unix socket address for [path], e.g. "/tmp/example.socket".
///
/// !return-ref
/// !return-type UtUnixSocketAddress
UtObject *ut_unix_socket_address_new(const char *path);

/// Returns the path this address is for.
const char *ut_unix_socket_address_get_path(UtObject *object);

/// Returns [true] if [object] is a [UtUnixSocketAddress].
bool ut_object_is_unix_socket_address(UtObject *object);
