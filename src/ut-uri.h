#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

UtObject *ut_uri_new(const char *scheme, const char *user_info,
                     const char *host, uint16_t port, const char *path,
                     const char *query, const char *fragment);

UtObject *ut_uri_new_from_string(const char *uri);

const char *ut_uri_get_scheme(UtObject *object);

const char *ut_uri_get_user_info(UtObject *object);

const char *ut_uri_get_host(UtObject *object);

uint16_t ut_uri_get_port(UtObject *object);

const char *ut_uri_get_path(UtObject *object);

const char *ut_uri_get_query(UtObject *object);

const char *ut_uri_get_fragment(UtObject *object);

bool ut_object_is_uri(UtObject *object);
