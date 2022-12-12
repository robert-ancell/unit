#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"

#pragma once

typedef void (*UtX11ClientSyncInitializeCallback)(void *user_data,
                                                  uint8_t major_version,
                                                  uint8_t minor_version,
                                                  UtObject *error);
typedef void (*UtX11ClientSyncListSystemCountersCallback)(void *user_data,
                                                          UtObject *counters,
                                                          UtObject *error);
typedef void (*UtX11ClientSyncQueryCounterCallback)(void *user_data,
                                                    int64_t counter_value,
                                                    UtObject *error);
typedef void (*UtX11ClientSyncQueryAlarmCallback)(void *user_data,
                                                  uint32_t trigger,
                                                  int64_t delta, bool events,
                                                  uint8_t state,
                                                  UtObject *error);
typedef void (*UtX11ClientSyncGetPriorityCallback)(void *user_data,
                                                   int32_t priority,
                                                   UtObject *error);
typedef void (*UtX11ClientSyncQueryFenceCallback)(void *user_data,
                                                  bool triggered,
                                                  UtObject *error);

UtObject *ut_x11_sync_extension_new(UtObject *client, uint8_t major_opcode,
                                    uint8_t first_event, uint8_t first_error,
                                    const UtX11EventCallbacks *event_callbacks,
                                    void *user_data, UtObject *cancel);

void ut_x11_sync_extension_initialize(
    UtObject *object, UtX11ClientSyncInitializeCallback callback,
    void *user_data, UtObject *cancel);

void ut_x11_sync_extension_list_system_counters(
    UtObject *object, UtX11ClientSyncListSystemCountersCallback callback,
    void *user_data, UtObject *cancel);

uint32_t ut_x11_sync_extension_create_counter(UtObject *object,
                                              int64_t initial_value);

void ut_x11_sync_extension_set_counter(UtObject *object, uint32_t counter,
                                       int64_t amount);

void ut_x11_sync_extension_change_counter(UtObject *object, uint32_t counter,
                                          int64_t amount);

void ut_x11_sync_extension_query_counter(
    UtObject *object, uint32_t counter,
    UtX11ClientSyncQueryCounterCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_sync_extension_destroy_counter(UtObject *object, uint32_t counter);

void ut_x11_sync_extension_await(UtObject *object, UtObject *wait_list);

uint32_t ut_x11_sync_extension_create_alarm(UtObject *object);

void ut_x11_sync_extension_change_alarm(UtObject *object, uint32_t alarm);

void ut_x11_sync_extension_query_alarm(
    UtObject *object, uint32_t alarm,
    UtX11ClientSyncQueryAlarmCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_sync_extension_destroy_alarm(UtObject *object, uint32_t alarm);

void ut_x11_sync_extension_set_priority(UtObject *object, uint32_t id,
                                        int32_t priority);

void ut_x11_sync_extension_get_priority(
    UtObject *object, uint32_t id, UtX11ClientSyncGetPriorityCallback callback,
    void *user_data, UtObject *cancel);

uint32_t ut_x11_sync_extension_create_fence(UtObject *object, uint32_t drawable,
                                            bool initially_triggered);

void ut_x11_sync_extension_trigger_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_reset_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_destroy_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_query_fence(
    UtObject *object, uint32_t fence,
    UtX11ClientSyncQueryFenceCallback callback, void *user_data,
    UtObject *cancel);

void ut_x11_sync_extension_await_fence(UtObject *object, uint32_t fence);

bool ut_object_is_x11_sync_extension(UtObject *object);
