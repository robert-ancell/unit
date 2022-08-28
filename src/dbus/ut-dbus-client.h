#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtDBusMethodRequestCallback)(void *user_data,
                                            UtObject *method_call);

typedef void (*UtDBusMethodResponseCallback)(void *user_data,
                                             UtObject *out_args);

UtObject *ut_dbus_client_new(const char *address);

UtObject *ut_dbus_client_new_system();

UtObject *ut_dbus_client_new_session();

void ut_dbus_client_set_method_call_handler(
    UtObject *object, UtDBusMethodRequestCallback callback, void *user_data,
    UtObject *cancel);

const char *ut_dbus_client_get_unique_name(UtObject *object);

void ut_dbus_client_call_method(UtObject *object, const char *destination,
                                const char *path, const char *interface,
                                const char *name, UtObject *args,
                                UtDBusMethodResponseCallback callback,
                                void *user_data, UtObject *cancel);

void ut_dbus_client_send_reply(UtObject *object, UtObject *method_call,
                               UtObject *args);

bool ut_object_is_dbus_client(UtObject *object);
