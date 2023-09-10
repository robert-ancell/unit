#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

/// Creates a new [UtUri].
///
/// !return-ref
UtObject *ut_uri_new(const char *scheme, const char *user_info,
                     const char *host, uint16_t port, const char *path,
                     const char *query, const char *fragment);

/// Creates a new [UtUri] from [uri], e.g. "http://example.com".
///
/// !return-ref
UtObject *ut_uri_new_from_string(const char *uri);

/// Returns the scheme in this URI, e.g. "http" for "http://example.com".
const char *ut_uri_get_scheme(UtObject *object);

/// Returns the user information in this URI, e.g. "user" for
/// "http://user@example.com".
const char *ut_uri_get_user_info(UtObject *object);

/// Returns the host in this URI, e.g. "example.com" for "http://example.com".
const char *ut_uri_get_host(UtObject *object);

/// Returns the port in this URI, e.g. 80 for "http://example.com:80".
uint16_t ut_uri_get_port(UtObject *object);

/// Returns the path in this URI, e.g. "/index.html" for
/// "http://example.com/index.html".
const char *ut_uri_get_path(UtObject *object);

/// Returns the query in this URI, e.g. "verbose=true" for
/// "http://example.com/index.html?verbose=true".
const char *ut_uri_get_query(UtObject *object);

/// Returns the fragment in this URI, e.g. "about" for
/// "http://example.com/index.html#about".
const char *ut_uri_get_fragment(UtObject *object);

/// Returns [true] if [object] is a [UtUri].
bool ut_object_is_uri(UtObject *object);
