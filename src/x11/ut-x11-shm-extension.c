#include <assert.h>
#include <unistd.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-shm-extension.h"
#include "ut.h"

typedef struct _UtX11ShmExtension UtX11ShmExtension;

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
    .type_name = "ShmCallbackData", .cleanup = callback_data_cleanup};

static UtObject *callback_data_new(UtObject *callback_object, void *callback) {
  UtObject *object =
      ut_object_new(sizeof(CallbackData), &callback_data_object_interface);
  CallbackData *self = (CallbackData *)object;
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

struct _UtX11ShmExtension {
  UtObject object;
  UtObject *client;
  uint8_t major_opcode;
  uint8_t first_event;
  uint8_t first_error;
};

static void decode_shm_enable_reply(UtObject *object, uint8_t data0,
                                    UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  bool shared_pixmaps = data0 != 0;
  uint16_t major_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t minor_version = ut_x11_buffer_get_card16(data, &offset);
  uint16_t uid = ut_x11_buffer_get_card16(data, &offset);
  uint16_t gid = ut_x11_buffer_get_card16(data, &offset);
  uint8_t pixmap_format = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 15);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShmQueryVersionCallback callback =
        (UtX11ShmQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, major_version, minor_version, uid,
             gid, pixmap_format, shared_pixmaps, NULL);
  }
}

static void handle_shm_enable_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShmQueryVersionCallback callback =
        (UtX11ShmQueryVersionCallback)callback_data->callback;
    callback(callback_data->callback_object, 0, 0, 0, 0, 0, false, error);
  }
}

static void decode_shm_create_segment_reply(UtObject *object, uint8_t data0,
                                            UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t fds_length = data0;
  assert(ut_x11_buffer_get_fd_count(data) >= fds_length);
  UtObjectRef fd = ut_x11_buffer_take_fd(data);

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShmCreateSegmentCallback callback =
        (UtX11ShmCreateSegmentCallback)callback_data->callback;
    callback(callback_data->callback_object, fd, NULL);
  }
}

static void handle_shm_create_segment_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback_object != NULL &&
      callback_data->callback != NULL) {
    UtX11ShmCreateSegmentCallback callback =
        (UtX11ShmCreateSegmentCallback)callback_data->callback;
    callback(callback_data->callback_object, NULL, error);
  }
}

static uint8_t ut_x11_shm_extension_get_major_opcode(UtObject *object) {
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;
  return self->major_opcode;
}

static uint8_t ut_x11_shm_extension_get_first_event(UtObject *object) {
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;
  return self->first_event;
}

static UtX11ErrorCode ut_x11_shm_extension_decode_error(UtObject *object,
                                                        uint8_t code) {
  switch (code) {
  case 0:
    return UT_X11_ERROR_BAD_SEGMENT;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_shm_extension_close(UtObject *object) {
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_shm_extension_get_major_opcode,
    .get_first_event = ut_x11_shm_extension_get_first_event,
    .decode_error = ut_x11_shm_extension_decode_error,
    .close = ut_x11_shm_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11ShmExtension",
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *ut_x11_shm_extension_new(UtObject *client, uint8_t major_opcode,
                                   uint8_t first_event, uint8_t first_error) {
  UtObject *object =
      ut_object_new(sizeof(UtX11ShmExtension), &object_interface);
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  return object;
}

void ut_x11_shm_extension_query_version(UtObject *object,
                                        UtObject *callback_object,
                                        UtX11ShmQueryVersionCallback callback) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, NULL,
      callback_data_new(callback_object, callback), decode_shm_enable_reply,
      handle_shm_enable_error);
}

uint32_t ut_x11_shm_extension_attach(UtObject *object, uint32_t shmid,
                                     bool read_only) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, shmid);
  ut_x11_buffer_append_bool(request, read_only);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_client_send_request(self->client, self->major_opcode, 1, request);

  return id;
}

void ut_x11_shm_extension_detach(UtObject *object, uint32_t segment) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, segment);

  ut_x11_client_send_request(self->client, self->major_opcode, 2, request);
}

uint32_t ut_x11_shm_extension_create_pixmap(UtObject *object, uint32_t drawable,
                                            uint16_t width, uint16_t height,
                                            uint8_t depth, uint32_t segment,
                                            uint32_t offset) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_card8(request, depth);
  ut_x11_buffer_append_padding(request, 3);
  ut_x11_buffer_append_card32(request, segment);
  ut_x11_buffer_append_card32(request, offset);

  ut_x11_client_send_request(self->client, self->major_opcode, 5, request);

  return id;
}

uint32_t ut_x11_shm_extension_attach_fd(UtObject *object, UtObject *fd,
                                        bool read_only) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_bool(request, read_only);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_buffer_append_fd(request, fd);

  ut_x11_client_send_request(self->client, self->major_opcode, 6, request);

  return id;
}

uint32_t
ut_x11_shm_extension_create_segment(UtObject *object, uint32_t size,
                                    bool read_only, UtObject *callback_object,
                                    UtX11ShmCreateSegmentCallback callback) {
  assert(ut_object_is_x11_shm_extension(object));
  UtX11ShmExtension *self = (UtX11ShmExtension *)object;

  uint32_t id = ut_x11_client_create_resource_id(self->client);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, size);
  ut_x11_buffer_append_bool(request, read_only);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 7, request,
      callback_data_new(callback_object, callback),
      decode_shm_create_segment_reply, handle_shm_create_segment_error);

  return id;
}

bool ut_object_is_x11_shm_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
