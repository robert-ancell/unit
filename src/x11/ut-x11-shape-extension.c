#include <assert.h>
#include <unistd.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-shape-extension.h"
#include "ut.h"

typedef struct _UtX11ShapeExtension UtX11ShapeExtension;

typedef struct {
  UtObject object;
  UtObject *callback_object;
  void *callback;
} CallbackData;

static void callback_data_cleanup(UtObject *object) {
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_unref(&self->callback_object);
}

static UtObjectInterface callback_data_object_interface = {
    .type_name = "ShapeCallbackData", .cleanup = callback_data_cleanup};

static UtObject *callback_data_new(UtObject *callback_object, void *callback) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

struct _UtX11ShapeExtension {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
  uint8_t first_event;
  UtObject *callback_object;
  const UtX11ShapeEventCallbacks *event_callbacks;
};

static void decode_shape_notify(UtX11ShapeExtension *self, uint8_t data0,
                                UtObject *data) {
  uint8_t kind = data0;
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  bool shaped = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 11);

  if (self->callback_object != NULL &&
      self->event_callbacks->shape_notify != NULL) {
    self->event_callbacks->shape_notify(self->callback_object, window, kind, x,
                                        y, width, height, timestamp, shaped);
  }
}

static void decode_shape_query_version_reply(UtObject *object, uint8_t data0,
                                             UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShapeQueryVersionCallback callback =
        (UtX11ShapeQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, major_version, minor_version,
             NULL);
  }
}

static void handle_shape_query_version_error(UtObject *object,
                                             UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShapeQueryVersionCallback callback =
        (UtX11ShapeQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, error);
  }
}

static void ut_x11_shape_extension_cleanup(UtObject *object) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  ut_object_weak_unref(&self->callback_object);
}

static uint8_t ut_x11_shape_extension_get_major_opcode(UtObject *object) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  return self->major_opcode;
}

static uint8_t ut_x11_shape_extension_get_first_event(UtObject *object) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  return self->first_event;
}

static bool ut_x11_shape_extension_decode_event(UtObject *object, uint8_t code,
                                                bool from_send_event,
                                                uint16_t sequence_number,
                                                uint8_t data0, UtObject *data) {
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  switch (code) {
  case 0:
    decode_shape_notify(self, data0, data);
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
    .get_major_opcode = ut_x11_shape_extension_get_major_opcode,
    .get_first_event = ut_x11_shape_extension_get_first_event,
    .decode_event = ut_x11_shape_extension_decode_event,
    .close = ut_x11_shape_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11ShapeExtension",
    .cleanup = ut_x11_shape_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_shape_extension_new(UtObject *client, uint8_t major_opcode,
                           uint8_t first_event, UtObject *callback_object,
                           const UtX11ShapeEventCallbacks *event_callbacks) {
  UtObject *object =
      ut_object_new(sizeof(UtX11ShapeExtension), &object_interface);
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->event_callbacks = event_callbacks;
  return object;
}

void ut_x11_shape_extension_query_version(
    UtObject *object, UtObject *callback_object,
    UtX11ShapeQueryVersionCallback callback) {
  assert(ut_object_is_x11_shape_extension(object));
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, NULL,
      callback_data_new(callback_object, callback),
      decode_shape_query_version_reply, handle_shape_query_version_error);
}

void ut_x11_shape_extension_select_input(UtObject *object, uint32_t window,
                                         bool enabled) {
  assert(ut_object_is_x11_shape_extension(object));
  UtX11ShapeExtension *self = (UtX11ShapeExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card8(request, enabled ? 1 : 0);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_client_send_request(self->client, self->major_opcode, 6, request);
}

bool ut_object_is_x11_shape_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
