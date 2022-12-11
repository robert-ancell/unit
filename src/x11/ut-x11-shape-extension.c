#include <assert.h>
#include <unistd.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut.h"

typedef struct _UtX11ShapeExtension UtX11ShapeExtension;

typedef struct {
  UtObject object;
  UtX11ShapeExtension *self;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "CallbackData", .interfaces = {{NULL, NULL}}};

static UtObject *callback_data_new(UtX11ShapeExtension *self, void *callback,
                                   void *user_data) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *data = (CallbackData *)object;
  data->self = self;
  data->callback = callback;
  data->user_data = user_data;
  return object;
}

struct _UtX11ShapeExtension {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
  uint8_t first_event;
  const UtX11ShapeEventCallbacks *event_callbacks;
  void *user_data;
  UtObject *cancel;
};

static void decode_shape_notify(UtX11ShapeExtension *self, UtObject *data) {
  size_t offset = 0;
  ut_x11_buffer_get_card8(data, &offset); // code
  uint8_t kind = ut_x11_buffer_get_card8(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  bool shaped = ut_x11_buffer_get_card8(data, &offset) != 0;
  ut_x11_buffer_get_padding(data, &offset, 11);

  if (self->event_callbacks->shape_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->shape_notify(self->user_data, window, kind, x, y,
                                        width, height, timestamp, shaped);
  }
}

static void decode_shape_query_version_reply(UtObject *user_data, uint8_t data0,
                                             UtObject *data) {
  CallbackData *callback_data = (CallbackData *)user_data;

  size_t offset = 0;
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback != NULL) {
    UtX11ShapeQueryVersionCallback callback =
        (UtX11ShapeQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void handle_shape_query_version_error(UtObject *user_data,
                                             UtObject *error) {
  CallbackData *callback_data = (CallbackData *)user_data;

  if (callback_data->callback != NULL) {
    UtX11ShapeQueryVersionCallback callback =
        (UtX11ShapeQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static bool ut_x11_shape_extension_decode_event(UtObject *object,
                                                UtObject *data) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  size_t offset = 0;
  uint8_t code = ut_x11_buffer_get_card8(data, &offset) & 0x7f;
  if (code < self->first_event) {
    return false;
  }
  code -= self->first_event;

  switch (code) {
  case 0:
    decode_shape_notify(self, data);
    return true;
  default:
    return false;
  }
}

static void ut_x11_shape_extension_close(UtObject *object) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .decode_event = ut_x11_shape_extension_decode_event,
    .close = ut_x11_shape_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11ShapeExtension",
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_shape_extension_new(UtObject *client, uint8_t major_opcode,
                           uint8_t first_event,
                           const UtX11ShapeEventCallbacks *event_callbacks,
                           void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11ShapeExtension), &object_interface);
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_shape_extension_query_version(
    UtObject *object, UtX11ShapeQueryVersionCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_shape_extension(object));
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_client_send_request_with_reply(
      (UtObject *)self->client, self->major_opcode, 0, request,
      decode_shape_query_version_reply, handle_shape_query_version_error,
      callback_data_new(self, callback, user_data), cancel);
}

void ut_x11_shape_extension_select_input(UtObject *object, uint32_t window,
                                         bool enabled) {
  assert(ut_object_is_x11_shape_extension(object));
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card8(request, enabled ? 1 : 0);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_client_send_request((UtObject *)self->client, self->major_opcode, 6,
                             request);
}

bool ut_object_is_x11_shape_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
