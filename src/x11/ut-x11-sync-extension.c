#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-sync-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "SyncCallbackData"};

static UtObject *callback_data_new(void *callback, void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  self->callback = callback;
  self->user_data = user_data;
  return object;
}

typedef struct {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
  uint8_t first_event;
  uint8_t first_error;
  const UtX11EventCallbacks *event_callbacks;
  void *user_data;
  UtObject *cancel;
} UtX11SyncExtension;

static void initialize_reply_cb(UtObject *object, uint8_t data0,
                                UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint8_t major_version = ut_x11_buffer_get_card8(data, &offset);
  uint8_t minor_version = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);

  if (callback_data->callback != NULL) {
    UtX11ClientSyncInitializeCallback callback =
        (UtX11ClientSyncInitializeCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void initialize_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientSyncInitializeCallback callback =
        (UtX11ClientSyncInitializeCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void list_system_counters_reply_cb(UtObject *object, uint8_t data0,
                                          UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint32_t counters_length = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);
  UtObjectRef counters = ut_object_list_new();
  for (size_t i = 0; i < counters_length; i++) {
    // FIXME
  }

  if (callback_data->callback != NULL) {
    UtX11ListSystemCountersCallback callback =
        (UtX11ListSystemCountersCallback)callback_data->callback;
    callback(callback_data->user_data, counters, NULL);
  }
}

static void list_system_counters_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ListSystemCountersCallback callback =
        (UtX11ListSystemCountersCallback)callback_data->callback;
    callback(callback_data->user_data, NULL, error);
  }
}

static void query_counter_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  int64_t counter_value = ut_x11_buffer_get_int64(data, &offset);

  if (callback_data->callback != NULL) {
    UtX11QueryCounterCallback callback =
        (UtX11QueryCounterCallback)callback_data->callback;
    callback(callback_data->user_data, counter_value, NULL);
  }
}

static void query_counter_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11QueryCounterCallback callback =
        (UtX11QueryCounterCallback)callback_data->callback;
    callback(callback_data->user_data, 0, error);
  }
}

static void query_alarm_reply_cb(UtObject *object, uint8_t data0,
                                 UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint32_t trigger = ut_x11_buffer_get_card32(data, &offset);
  uint64_t delta = ut_x11_buffer_get_int64(data, &offset);
  bool events = ut_x11_buffer_get_bool(data, &offset);
  uint8_t state = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);

  if (callback_data->callback != NULL) {
    UtX11ClientSyncQueryAlarmCallback callback =
        (UtX11ClientSyncQueryAlarmCallback)callback_data->callback;
    callback(callback_data->user_data, trigger, delta, events, state, NULL);
  }
}

static void query_alarm_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientSyncQueryAlarmCallback callback =
        (UtX11ClientSyncQueryAlarmCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, 0, 0, error);
  }
}

static void get_priority_reply_cb(UtObject *object, uint8_t data0,
                                  UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  int32_t priority = ut_x11_buffer_get_int32(data, &offset);

  if (callback_data->callback != NULL) {
    UtX11ClientSyncGetPriorityCallback callback =
        (UtX11ClientSyncGetPriorityCallback)callback_data->callback;
    callback(callback_data->user_data, priority, NULL);
  }
}

static void get_priority_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientSyncGetPriorityCallback callback =
        (UtX11ClientSyncGetPriorityCallback)callback_data->callback;
    callback(callback_data->user_data, 0, error);
  }
}

static void query_fence_reply_cb(UtObject *object, uint8_t data0,
                                 UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  bool triggered = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 23);

  if (callback_data->callback != NULL) {
    UtX11ClientSyncQueryFenceCallback callback =
        (UtX11ClientSyncQueryFenceCallback)callback_data->callback;
    callback(callback_data->user_data, triggered, NULL);
  }
}

static void query_fence_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientSyncQueryFenceCallback callback =
        (UtX11ClientSyncQueryFenceCallback)callback_data->callback;
    callback(callback_data->user_data, false, error);
  }
}

static void ut_x11_sync_extension_cleanup(UtObject *object) {
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  ut_object_unref(self->cancel);
}

static uint8_t ut_x11_sync_extension_get_major_opcode(UtObject *object) {
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  return self->major_opcode;
}

static uint8_t ut_x11_sync_extension_get_first_event(UtObject *object) {
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  return self->first_event;
}

static uint8_t ut_x11_sync_extension_get_first_error(UtObject *object) {
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  return self->first_error;
}

static bool ut_x11_sync_extension_decode_event(UtObject *object, uint8_t code,
                                               bool from_send_event,
                                               uint16_t sequence_number,
                                               uint8_t data0, UtObject *data) {
  switch (code) {
  default:
    return false;
  }
}

static UtX11ErrorCode ut_x11_sync_extension_decode_error(UtObject *object,
                                                         uint8_t code) {
  switch (code) {
  case 0:
    return UT_X11_ERROR_COUNTER;
  case 1:
    return UT_X11_ERROR_ALARM;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_sync_extension_close(UtObject *object) {
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_sync_extension_get_major_opcode,
    .get_first_event = ut_x11_sync_extension_get_first_event,
    .get_first_error = ut_x11_sync_extension_get_first_error,
    .decode_event = ut_x11_sync_extension_decode_event,
    .decode_error = ut_x11_sync_extension_decode_error,
    .close = ut_x11_sync_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11SyncExtension",
    .cleanup = ut_x11_sync_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_sync_extension_new(UtObject *client, uint8_t major_opcode,
                                    uint8_t first_event, uint8_t first_error,
                                    const UtX11EventCallbacks *event_callbacks,
                                    void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11SyncExtension), &object_interface);
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_sync_extension_initialize(
    UtObject *object, UtX11ClientSyncInitializeCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card8(request, 3);
  ut_x11_buffer_append_card8(request, 1);
  ut_x11_buffer_append_padding(request, 2);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request, initialize_reply_cb,
      initialize_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_sync_extension_list_system_counters(
    UtObject *object, UtX11ListSystemCountersCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 1, NULL, list_system_counters_reply_cb,
      list_system_counters_error_cb, callback_data_new(callback, user_data),
      cancel);
}

uint32_t ut_x11_sync_extension_create_counter(UtObject *object,
                                              int64_t initial_value) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_int64(request, initial_value);

  ut_x11_client_send_request(self->client, self->major_opcode, 2, request);

  return id;
}

void ut_x11_sync_extension_set_counter(UtObject *object, uint32_t counter,
                                       int64_t amount) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, counter);
  ut_x11_buffer_append_int64(request, amount);

  ut_x11_client_send_request(self->client, self->major_opcode, 3, request);
}

void ut_x11_sync_extension_change_counter(UtObject *object, uint32_t counter,
                                          int64_t amount) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, counter);
  ut_x11_buffer_append_int32(request, amount);

  ut_x11_client_send_request(self->client, self->major_opcode, 4, request);
}

void ut_x11_sync_extension_query_counter(UtObject *object, uint32_t counter,
                                         UtX11QueryCounterCallback callback,
                                         void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, counter);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 5, request, query_counter_reply_cb,
      query_counter_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_sync_extension_destroy_counter(UtObject *object, uint32_t counter) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, counter);

  ut_x11_client_send_request(self->client, self->major_opcode, 6, request);
}

void ut_x11_sync_extension_await(UtObject *object, UtObject *wait_list) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  size_t wait_list_length = ut_list_get_length(wait_list);
  for (size_t i = 0; i < wait_list_length; i++) {
    // FIXME
    // ut_x11_buffer_append_card32(request, trigger);
    // ut_x11_buffer_append_int64(request, event_threshold);
  }

  ut_x11_client_send_request(self->client, self->major_opcode, 7, request);
}

uint32_t ut_x11_sync_extension_create_alarm(UtObject *object) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, 0);
  // FIXME: Values

  ut_x11_client_send_request(self->client, self->major_opcode, 8, request);

  return id;
}

void ut_x11_sync_extension_change_alarm(UtObject *object, uint32_t alarm) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, alarm);
  ut_x11_buffer_append_card32(request, 0);
  // FIXME: Values

  ut_x11_client_send_request(self->client, self->major_opcode, 9, request);
}

void ut_x11_sync_extension_query_alarm(
    UtObject *object, uint32_t alarm,
    UtX11ClientSyncQueryAlarmCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, alarm);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 10, request, query_alarm_reply_cb,
      query_alarm_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_sync_extension_destroy_alarm(UtObject *object, uint32_t alarm) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, alarm);

  ut_x11_client_send_request(self->client, self->major_opcode, 11, request);
}

void ut_x11_sync_extension_set_priority(UtObject *object, uint32_t id,
                                        int32_t priority) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_int32(request, priority);

  ut_x11_client_send_request(self->client, self->major_opcode, 12, request);
}

void ut_x11_sync_extension_get_priority(
    UtObject *object, uint32_t id, UtX11ClientSyncGetPriorityCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 13, request, get_priority_reply_cb,
      get_priority_error_cb, callback_data_new(callback, user_data), cancel);
}

uint32_t ut_x11_sync_extension_create_fence(UtObject *object, uint32_t drawable,
                                            bool initially_triggered) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  uint32_t fence = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card32(request, fence);
  ut_x11_buffer_append_card8(request, initially_triggered ? 1 : 0);

  ut_x11_client_send_request(self->client, self->major_opcode, 14, request);

  return fence;
}

void ut_x11_sync_extension_trigger_fence(UtObject *object, uint32_t fence) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, fence);

  ut_x11_client_send_request(self->client, self->major_opcode, 15, request);
}

void ut_x11_sync_extension_reset_fence(UtObject *object, uint32_t fence) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, fence);

  ut_x11_client_send_request(self->client, self->major_opcode, 16, request);
}

void ut_x11_sync_extension_destroy_fence(UtObject *object, uint32_t fence) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, fence);

  ut_x11_client_send_request(self->client, self->major_opcode, 17, request);
}

void ut_x11_sync_extension_query_fence(
    UtObject *object, uint32_t fence,
    UtX11ClientSyncQueryFenceCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, fence);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 18, request, query_fence_reply_cb,
      query_fence_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_sync_extension_await_fence(UtObject *object, uint32_t fence) {
  assert(ut_object_is_x11_sync_extension(object));
  UtX11SyncExtension *self = (UtX11SyncExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, fence);

  ut_x11_client_send_request(self->client, self->major_opcode, 19, request);
}

bool ut_object_is_x11_sync_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
