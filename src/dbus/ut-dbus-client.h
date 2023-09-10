#include <stdbool.h>

#include "ut-object.h"

#pragma once

typedef void (*UtDBusMethodRequestCallback)(UtObject *object,
                                            UtObject *method_call);

typedef void (*UtDBusMethodResponseCallback)(UtObject *object,
                                             UtObject *out_args);

UtObject *ut_dbus_client_new(const char *address);

UtObject *ut_dbus_client_new_system();

UtObject *ut_dbus_client_new_session();

void ut_dbus_client_set_method_call_handler(
    UtObject *object, UtObject *callback_object,
    UtDBusMethodRequestCallback callback);

const char *ut_dbus_client_get_unique_name(UtObject *object);

void ut_dbus_client_call_method(UtObject *object, const char *destination,
                                const char *path, const char *interface,
                                const char *name, UtObject *args,
                                UtObject *callback_object,
                                UtDBusMethodResponseCallback callback);

void ut_dbus_client_send_reply(UtObject *object, UtObject *method_call,
                               UtObject *args);

/// Returns [true] if [object] is a [UtDbusClient].
bool ut_object_is_dbus_client(UtObject *object);
