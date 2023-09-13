#include <stdbool.h>
#include <stdint.h>

#include "ut-object.h"
#include "ut-x11-client.h"

#pragma once

typedef void (*UtX11ClientSyncInitializeCallback)(UtObject *object,
                                                  uint8_t major_version,
                                                  uint8_t minor_version,
                                                  UtObject *error);
typedef void (*UtX11ClientSyncListSystemCountersCallback)(UtObject *object,
                                                          UtObject *counters,
                                                          UtObject *error);
typedef void (*UtX11ClientSyncQueryCounterCallback)(UtObject *object,
                                                    int64_t counter_value,
                                                    UtObject *error);
typedef void (*UtX11ClientSyncQueryAlarmCallback)(UtObject *object,
                                                  uint32_t trigger,
                                                  int64_t delta, bool events,
                                                  uint8_t state,
                                                  UtObject *error);
typedef void (*UtX11ClientSyncGetPriorityCallback)(UtObject *object,
                                                   int32_t priority,
                                                   UtObject *error);
typedef void (*UtX11ClientSyncQueryFenceCallback)(UtObject *object,
                                                  bool triggered,
                                                  UtObject *error);

/// Creates a new X11 sync extension.
///
/// !arg-type client UtX11Client
/// !return-ref
/// !return-type UtX11SyncExtension
UtObject *ut_x11_sync_extension_new(UtObject *client, uint8_t major_opcode,
                                    uint8_t first_event, uint8_t first_error,
                                    UtObject *callback_object,
                                    const UtX11EventCallbacks *event_callbacks);

void ut_x11_sync_extension_initialize(
    UtObject *object, UtObject *callback_object,
    UtX11ClientSyncInitializeCallback callback);

void ut_x11_sync_extension_list_system_counters(
    UtObject *object, UtObject *callback_object,
    UtX11ClientSyncListSystemCountersCallback callback);

uint32_t ut_x11_sync_extension_create_counter(UtObject *object,
                                              int64_t initial_value);

void ut_x11_sync_extension_set_counter(UtObject *object, uint32_t counter,
                                       int64_t amount);

void ut_x11_sync_extension_change_counter(UtObject *object, uint32_t counter,
                                          int64_t amount);

void ut_x11_sync_extension_query_counter(
    UtObject *object, uint32_t counter, UtObject *callback_object,
    UtX11ClientSyncQueryCounterCallback callback);

void ut_x11_sync_extension_destroy_counter(UtObject *object, uint32_t counter);

void ut_x11_sync_extension_await(UtObject *object, UtObject *wait_list);

uint32_t ut_x11_sync_extension_create_alarm(UtObject *object);

void ut_x11_sync_extension_change_alarm(UtObject *object, uint32_t alarm);

void ut_x11_sync_extension_query_alarm(
    UtObject *object, uint32_t alarm, UtObject *callback_object,
    UtX11ClientSyncQueryAlarmCallback callback);

void ut_x11_sync_extension_destroy_alarm(UtObject *object, uint32_t alarm);

void ut_x11_sync_extension_set_priority(UtObject *object, uint32_t id,
                                        int32_t priority);

void ut_x11_sync_extension_get_priority(
    UtObject *object, uint32_t id, UtObject *callback_object,
    UtX11ClientSyncGetPriorityCallback callback);

uint32_t ut_x11_sync_extension_create_fence(UtObject *object, uint32_t drawable,
                                            bool initially_triggered);

void ut_x11_sync_extension_trigger_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_reset_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_destroy_fence(UtObject *object, uint32_t fence);

void ut_x11_sync_extension_query_fence(
    UtObject *object, uint32_t fence, UtObject *callback_object,
    UtX11ClientSyncQueryFenceCallback callback);

void ut_x11_sync_extension_await_fence(UtObject *object, uint32_t fence);

/// Returns [true] if [object] is a [UtX11SyncExtension].
bool ut_object_is_x11_sync_extension(UtObject *object);
