#include <assert.h>
#include <unistd.h>

#include "ut-wayland-client-private.h"
#include "ut-wayland-object-private.h"
#include "ut.h"

// https://wayland.freedesktop.org/

typedef struct {
  UtObject object;
  UtObject *socket;

  uint32_t next_id;

  UtObject *objects;
  UtObject *display;
} UtWaylandClient;

static UtObject *find_object(UtWaylandClient *self, uint32_t id) {
  size_t objects_length = ut_list_get_length(self->objects);
  for (size_t i = 0; i < objects_length; i++) {
    UtObject *object = ut_object_list_get_element(self->objects, i);
    if (ut_wayland_object_get_id(object) == id) {
      return object;
    }
  }

  return NULL;
}

static void decode_event(UtWaylandClient *self, uint32_t id, uint16_t code,
                         UtObject *data) {
  UtObject *object = find_object(self, id);
  if (object != NULL) {
    if (!ut_wayland_object_decode_event(object, code, data)) {
      // printf("%s::%d %s\n", ut_wayland_object_get_interface(object), code,
      // ut_object_to_string(ut_wayland_decoder_get_data(data)));
    }
  } else {
    // printf("%d::%d %s\n", id, code,
    // ut_object_to_string(ut_wayland_decoder_get_data(data)));
  }
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtWaylandClient *self = (UtWaylandClient *)object;

  size_t data_length = ut_list_get_length(data);
  size_t offset = 0;
  while (true) {
    if (data_length < offset + 8) {
      if (complete) {
        // FIXME: Unused data is length > 0
        ut_input_stream_close(self->socket);
      }
      return offset;
    }

    UtObjectRef header_data = ut_list_get_sublist(data, offset, 8);
    UtObjectRef header = ut_wayland_decoder_new(header_data);
    uint32_t id = ut_wayland_decoder_get_uint(header);
    uint32_t payload_length_and_code = ut_wayland_decoder_get_uint(header);
    uint16_t payload_length = payload_length_and_code >> 16;
    uint16_t code = payload_length_and_code & 0xffff;

    if (ut_list_get_length(data) < offset + payload_length) {
      if (complete) {
        // FIXME: Missing data
        ut_input_stream_close(self->socket);
      }
      return offset;
    }

    UtObjectRef payload_data =
        ut_list_get_sublist(data, offset + 8, payload_length - 8);
    UtObjectRef payload = ut_wayland_decoder_new(payload_data);
    decode_event(self, id, code, payload);

    offset += payload_length;
  }
}

static void error_cb(UtObject *object, uint32_t id, uint32_t code,
                     const char *message) {}

static void delete_id_cb(UtObject *object, uint32_t id) {
  UtWaylandClient *self = (UtWaylandClient *)object;

  size_t objects_length = ut_list_get_length(self->objects);
  for (size_t i = 0; i < objects_length; i++) {
    UtObject *object = ut_object_list_get_element(self->objects, i);
    if (ut_wayland_object_get_id(object) == id) {
      ut_list_remove(self->objects, i, 1);
      return;
    }
  }
}

static UtWaylandDisplayCallbacks display_callbacks = {
    .error = error_cb, .delete_id = delete_id_cb};

static void ut_wayland_client_init(UtObject *object) {
  UtWaylandClient *self = (UtWaylandClient *)object;
  self->next_id = 2;
  self->objects = ut_object_list_new();
}

static void ut_wayland_client_cleanup(UtObject *object) {
  UtWaylandClient *self = (UtWaylandClient *)object;

  ut_input_stream_close(self->socket);

  ut_object_unref(self->socket);
  ut_object_unref(self->objects);
  ut_object_unref(self->display);
}

static UtObjectInterface object_interface = {.type_name = "UtWaylandClient",
                                             .init = ut_wayland_client_init,
                                             .cleanup =
                                                 ut_wayland_client_cleanup};

UtObject *ut_wayland_client_new() {
  UtObject *object = ut_object_new(sizeof(UtWaylandClient), &object_interface);
  UtWaylandClient *self = (UtWaylandClient *)object;

  const char *wayland_display = getenv("WAYLAND_DISPLAY");
  if (wayland_display == NULL) {
    wayland_display = "wayland-0";
  }
  const char *runtime_dir = getenv("XDG_RUNTIME_DIR");
  ut_cstring_ref dir = NULL;
  if (runtime_dir == NULL) {
    uid_t uid = getuid();
    dir = ut_cstring_new_printf("/run/user/%d", uid);
    runtime_dir = dir;
  }
  ut_cstring_ref path =
      ut_cstring_new_printf("%s/%s", runtime_dir, wayland_display);
  UtObjectRef address = ut_unix_socket_address_new(path);
  self->socket = ut_tcp_socket_new(address, 0);
  return object;
}

void ut_wayland_client_connect(UtObject *object) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;

  ut_tcp_socket_connect(self->socket, NULL, NULL);
  ut_input_stream_read(self->socket, object, read_cb);

  self->display = ut_wayland_display_new(object, object, &display_callbacks);
}

UtObject *ut_wayland_client_sync(UtObject *object, UtObject *callback_object,
                                 UtWaylandCallbackDoneCallback done_callback) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;
  return ut_wayland_display_sync(self->display, callback_object, done_callback);
}

UtObject *
ut_wayland_client_get_registry(UtObject *object, UtObject *callback_object,
                               const UtWaylandRegistryCallbacks *callbacks) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;
  return ut_wayland_display_get_registry(self->display, callback_object,
                                         callbacks);
}

uint32_t ut_wayland_client_allocate_id(UtObject *object) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;

  uint32_t id = self->next_id;
  self->next_id++;

  return id;
}

void ut_wayland_client_register_object(UtObject *object,
                                       UtObject *wayland_object) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;
  ut_list_append(self->objects, wayland_object);
}

UtObject *ut_wayland_client_get_object(UtObject *object, uint32_t id) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;
  return find_object(self, id);
}

void ut_wayland_client_send_request(UtObject *object, uint32_t id,
                                    uint16_t code, UtObject *payload) {
  assert(ut_object_is_wayland_client(object));
  UtWaylandClient *self = (UtWaylandClient *)object;

  UtObjectRef request = ut_wayland_encoder_new();
  uint16_t request_length = 8 + ut_list_get_length(payload);
  ut_wayland_encoder_append_uint(request, id);
  ut_wayland_encoder_append_uint(request, request_length << 16 | code);
  ut_output_stream_write(self->socket, ut_wayland_encoder_get_data(request));
  ut_output_stream_write(self->socket, payload);
}

bool ut_object_is_wayland_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
