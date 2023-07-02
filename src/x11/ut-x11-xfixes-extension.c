#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut-x11-xfixes-extension.h"
#include "ut.h"

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "XfixesCallbackData"};

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
} UtX11XfixesExtension;

static void query_version_reply_cb(UtObject *object, uint8_t data0,
                                   UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint32_t major_version = ut_x11_buffer_get_card32(data, &offset);
  uint32_t minor_version = ut_x11_buffer_get_card32(data, &offset);

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesQueryVersionCallback callback =
        (UtX11ClientXfixesQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, major_version, minor_version, NULL);
  }
}

static void query_version_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesQueryVersionCallback callback =
        (UtX11ClientXfixesQueryVersionCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, error);
  }
}

static void get_cursor_image_reply_cb(UtObject *object, uint8_t data0,
                                      UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  uint16_t xhot = ut_x11_buffer_get_card16(data, &offset);
  uint16_t yhot = ut_x11_buffer_get_card16(data, &offset);
  uint32_t cursor_serial = ut_x11_buffer_get_card32(data, &offset);
  UtObjectRef cursor_image = ut_uint32_list_new();
  size_t cursor_image_length = width * height;
  for (size_t i = 0; i < cursor_image_length; i++) {
    ut_uint32_list_append(cursor_image,
                          ut_x11_buffer_get_card32(data, &offset));
  }

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorImageCallback callback =
        (UtX11ClientXfixesGetCursorImageCallback)callback_data->callback;
    callback(callback_data->user_data, x, y, width, height, xhot, yhot,
             cursor_serial, cursor_image, NULL);
  }
}

static void get_cursor_image_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorImageCallback callback =
        (UtX11ClientXfixesGetCursorImageCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, 0, 0, 0, 0, 0, NULL, error);
  }
}

static void get_cursor_name_reply_cb(UtObject *object, uint8_t data0,
                                     UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  uint32_t atom = ut_x11_buffer_get_card32(data, &offset);
  uint16_t name_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 18);
  ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorNameCallback callback =
        (UtX11ClientXfixesGetCursorNameCallback)callback_data->callback;
    callback(callback_data->user_data, atom, name, NULL);
  }
}

static void get_cursor_name_error_cb(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorNameCallback callback =
        (UtX11ClientXfixesGetCursorNameCallback)callback_data->callback;
    callback(callback_data->user_data, 0, NULL, error);
  }
}

static void get_cursor_image_and_name_reply_cb(UtObject *object, uint8_t data0,
                                               UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 1);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  uint16_t xhot = ut_x11_buffer_get_card16(data, &offset);
  uint16_t yhot = ut_x11_buffer_get_card16(data, &offset);
  uint32_t cursor_serial = ut_x11_buffer_get_card32(data, &offset);
  uint32_t cursor_atom = ut_x11_buffer_get_card32(data, &offset);
  uint16_t name_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 2);
  ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
  UtObjectRef cursor_image = ut_uint32_list_new();
  size_t cursor_image_length = width * height;
  for (size_t i = 0; i < cursor_image_length; i++) {
    ut_uint32_list_append(cursor_image,
                          ut_x11_buffer_get_card32(data, &offset));
  }

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorImageAndNameCallback callback =
        (UtX11ClientXfixesGetCursorImageAndNameCallback)callback_data->callback;
    callback(callback_data->user_data, x, y, width, height, xhot, yhot,
             cursor_serial, cursor_image, cursor_atom, name, NULL);
  }
}

static void get_cursor_image_and_name_error_cb(UtObject *object,
                                               UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ClientXfixesGetCursorImageAndNameCallback callback =
        (UtX11ClientXfixesGetCursorImageAndNameCallback)callback_data->callback;
    callback(callback_data->user_data, 0, 0, 0, 0, 0, 0, 0, NULL, 0, NULL,
             error);
  }
}

static void decode_selection_notify(UtX11XfixesExtension *self, uint8_t data0,
                                    UtObject *data) {
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t owner = ut_x11_buffer_get_card32(data, &offset);
  uint32_t selection = ut_x11_buffer_get_card32(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  uint32_t selection_timestamp = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 8);

  if (self->event_callbacks->selection_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->selection_notify(self->user_data, window, owner,
                                            selection, timestamp,
                                            selection_timestamp);
  }
}

static void decode_cursor_notify(UtX11XfixesExtension *self, uint8_t data0,
                                 UtObject *data) {
  size_t offset = 0;
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t cursor_serial = ut_x11_buffer_get_card32(data, &offset);
  uint32_t timestamp = ut_x11_buffer_get_card32(data, &offset);
  uint32_t name = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 12);

  if (self->event_callbacks->cursor_notify != NULL &&
      !ut_cancel_is_active(self->cancel)) {
    self->event_callbacks->cursor_notify(self->user_data, window, cursor_serial,
                                         timestamp, name);
  }
}

static void ut_x11_xfixes_extension_cleanup(UtObject *object) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  ut_object_unref(self->cancel);
}

static uint8_t ut_x11_xfixes_extension_get_major_opcode(UtObject *object) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  return self->major_opcode;
}

static uint8_t ut_x11_xfixes_extension_get_first_event(UtObject *object) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  return self->first_event;
}

static uint8_t ut_x11_xfixes_extension_get_first_error(UtObject *object) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  return self->first_error;
}

static bool ut_x11_xfixes_extension_decode_event(UtObject *object, uint8_t code,
                                                 bool from_send_event,
                                                 uint16_t sequence_number,
                                                 uint8_t data0,
                                                 UtObject *data) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  switch (code) {
  case 0:
    decode_selection_notify(self, data0, data);
    return true;
  case 1:
    decode_cursor_notify(self, data0, data);
    return true;
  default:
    return false;
  }
}

static UtX11ErrorCode ut_x11_xfixes_extension_decode_error(UtObject *object,
                                                           uint8_t code) {
  switch (code) {
  case 0:
    return UT_X11_ERROR_BAD_REGION;
  default:
    return UT_X11_ERROR_UNKNOWN;
  }
}

static void ut_x11_xfixes_extension_close(UtObject *object) {
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  self->client = NULL;
}

static UtX11ExtensionInterface x11_extension_interface = {
    .get_major_opcode = ut_x11_xfixes_extension_get_major_opcode,
    .get_first_event = ut_x11_xfixes_extension_get_first_event,
    .get_first_error = ut_x11_xfixes_extension_get_first_error,
    .decode_event = ut_x11_xfixes_extension_decode_event,
    .decode_error = ut_x11_xfixes_extension_decode_error,
    .close = ut_x11_xfixes_extension_close};

static UtObjectInterface object_interface = {
    .type_name = "UtX11XfixesExtension",
    .cleanup = ut_x11_xfixes_extension_cleanup,
    .interfaces = {{&ut_x11_extension_id, &x11_extension_interface},
                   {NULL, NULL}}};

UtObject *
ut_x11_xfixes_extension_new(UtObject *client, uint8_t major_opcode,
                            uint8_t first_event, uint8_t first_error,
                            const UtX11EventCallbacks *event_callbacks,
                            void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(UtX11XfixesExtension), &object_interface);
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  self->client = client;
  self->major_opcode = major_opcode;
  self->first_event = first_event;
  self->first_error = first_error;
  self->event_callbacks = event_callbacks;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_xfixes_extension_query_version(
    UtObject *object, UtX11ClientXfixesQueryVersionCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, 1);
  ut_x11_buffer_append_card32(request, 0);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 0, request, query_version_reply_cb,
      query_version_error_cb, callback_data_new(callback, user_data), cancel);
}

// ChangeSaveSet

void ut_x11_xfixes_select_selection_input(UtObject *object, uint32_t window,
                                          uint32_t selection,
                                          uint32_t event_mask) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, selection);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(object, self->major_opcode, 2, request);
}

void ut_x11_xfixes_select_cursor_input(UtObject *object, uint32_t window,
                                       uint32_t event_mask) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(object, self->major_opcode, 3, request);
}

void ut_x11_xfixes_extension_get_cursor_image(
    UtObject *object, UtX11ClientXfixesGetCursorImageCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 4, NULL, get_cursor_image_reply_cb,
      get_cursor_image_error_cb, callback_data_new(callback, user_data),
      cancel);
}

uint32_t ut_x11_xfixes_create_region(UtObject *object, UtObject *rectangles) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  uint32_t region = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  size_t rectangles_length = ut_list_get_length(rectangles);
  for (size_t i = 0; i < rectangles_length; i++) {
    UtObjectRef rectangle = ut_list_get_element(rectangles, i);
    int16_t x, y;
    uint16_t width, height;

    ut_x11_rectangle_get_dimensions(rectangle, &x, &y, &width, &height);
    ut_x11_buffer_append_int16(request, x);
    ut_x11_buffer_append_int16(request, y);
    ut_x11_buffer_append_card16(request, width);
    ut_x11_buffer_append_card16(request, height);
  }

  ut_x11_client_send_request(object, self->major_opcode, 5, request);

  return region;
}

uint32_t ut_x11_xfixes_create_region_from_bitmap(UtObject *object,
                                                 uint32_t bitmap) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  uint32_t region = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_card32(request, bitmap);

  ut_x11_client_send_request(object, self->major_opcode, 6, request);

  return region;
}

uint32_t ut_x11_xfixes_create_region_from_window(UtObject *object,
                                                 uint32_t window,
                                                 uint8_t kind) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  uint32_t region = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card8(request, kind);
  ut_x11_buffer_append_padding(request, 3);

  ut_x11_client_send_request(object, self->major_opcode, 7, request);

  return region;
}

uint32_t ut_x11_xfixes_create_region_from_gc(UtObject *object, uint32_t gc) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  uint32_t region = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_card32(request, gc);

  ut_x11_client_send_request(object, self->major_opcode, 8, request);

  return region;
}

uint32_t ut_x11_xfixes_create_region_from_picture(UtObject *object,
                                                  uint32_t picture) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;
  uint32_t region = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_card32(request, picture);

  ut_x11_client_send_request(object, self->major_opcode, 9, request);

  return region;
}

void ut_x11_xfixes_destroy_region(UtObject *object, uint32_t region) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);

  ut_x11_client_send_request(object, self->major_opcode, 10, request);
}

void ut_x11_xfixes_set_region(UtObject *object, uint32_t region,
                              UtObject *rectangles) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  size_t rectangles_length = ut_list_get_length(rectangles);
  for (size_t i = 0; i < rectangles_length; i++) {
    UtObjectRef rectangle = ut_list_get_element(rectangles, i);
    int16_t x, y;
    uint16_t width, height;

    ut_x11_rectangle_get_dimensions(rectangle, &x, &y, &width, &height);
    ut_x11_buffer_append_int16(request, x);
    ut_x11_buffer_append_int16(request, y);
    ut_x11_buffer_append_card16(request, width);
    ut_x11_buffer_append_card16(request, height);
  }

  ut_x11_client_send_request(object, self->major_opcode, 11, request);
}

void ut_x11_xfixes_copy_region(UtObject *object, uint32_t source,
                               uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 12, request);
}

void ut_x11_xfixes_union_region(UtObject *object, uint32_t source1,
                                uint32_t source2, uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source1);
  ut_x11_buffer_append_card32(request, source2);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 13, request);
}

void ut_x11_xfixes_intersect_region(UtObject *object, uint32_t source1,
                                    uint32_t source2, uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source1);
  ut_x11_buffer_append_card32(request, source2);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 14, request);
}

void ut_x11_xfixes_subtract_region(UtObject *object, uint32_t source1,
                                   uint32_t source2, uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source1);
  ut_x11_buffer_append_card32(request, source2);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 15, request);
}

void ut_x11_xfixes_invert_region(UtObject *object, uint32_t source,
                                 uint32_t bounds, uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source);
  ut_x11_buffer_append_card32(request, bounds);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 16, request);
}

void ut_x11_xfixes_translate_region(UtObject *object, uint32_t region,
                                    int16_t dx, int16_t dy) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_int16(request, dx);
  ut_x11_buffer_append_int16(request, dy);

  ut_x11_client_send_request(object, self->major_opcode, 17, request);
}

void ut_x11_xfixes_region_extents(UtObject *object, uint32_t source,
                                  uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 18, request);
}

// FetchRegion

void ut_x11_xfixes_set_gc_clip_region(UtObject *object, uint32_t gc,
                                      uint32_t region, int16_t x_origin,
                                      int16_t y_origin) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_int16(request, x_origin);
  ut_x11_buffer_append_int16(request, y_origin);

  ut_x11_client_send_request(object, self->major_opcode, 20, request);
}

void ut_x11_xfixes_set_window_shape_region(UtObject *object, uint32_t dest,
                                           uint8_t kind, int16_t x_offset,
                                           int16_t y_offset, uint32_t region) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, dest);
  ut_x11_buffer_append_card8(request, kind);
  ut_x11_buffer_append_padding(request, 3);
  ut_x11_buffer_append_int16(request, x_offset);
  ut_x11_buffer_append_int16(request, y_offset);
  ut_x11_buffer_append_card32(request, region);

  ut_x11_client_send_request(object, self->major_opcode, 21, request);
}

void ut_x11_xfixes_set_picture_clip_region(UtObject *object, uint32_t picture,
                                           uint32_t region, int16_t x_origin,
                                           int16_t y_origin) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, picture);
  ut_x11_buffer_append_card32(request, region);
  ut_x11_buffer_append_int16(request, x_origin);
  ut_x11_buffer_append_int16(request, y_origin);

  ut_x11_client_send_request(object, self->major_opcode, 21, request);
}

void ut_x11_xfixes_set_cursor_name(UtObject *object, uint32_t cursor,
                                   const char *name) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, cursor);
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);

  ut_x11_client_send_request(object, self->major_opcode, 23, request);
}

void ut_x11_xfixes_extension_get_cursor_name(
    UtObject *object, uint32_t cursor,
    UtX11ClientXfixesGetCursorNameCallback callback, void *user_data,
    UtObject *cancel) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, cursor);

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 24, request, get_cursor_name_reply_cb,
      get_cursor_name_error_cb, callback_data_new(callback, user_data), cancel);
}

void ut_x11_xfixes_extension_get_cursor_image_and_name(
    UtObject *object, UtX11ClientXfixesGetCursorImageAndNameCallback callback,
    void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  ut_x11_client_send_request_with_reply(
      self->client, self->major_opcode, 25, NULL,
      get_cursor_image_and_name_reply_cb, get_cursor_image_and_name_error_cb,
      callback_data_new(callback, user_data), cancel);
}

void ut_x11_xfixes_change_cursor(UtObject *object, uint32_t source,
                                 uint32_t destination) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source);
  ut_x11_buffer_append_card32(request, destination);

  ut_x11_client_send_request(object, self->major_opcode, 26, request);
}

void ut_x11_xfixes_change_cursor_by_name(UtObject *object, uint32_t cursor,
                                         const char *name) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, cursor);
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);

  ut_x11_client_send_request(object, self->major_opcode, 27, request);
}

void ut_x11_xfixes_expand_region(UtObject *object, uint32_t source,
                                 uint32_t destination, uint16_t left,
                                 uint16_t right, uint16_t top,
                                 uint16_t bottom) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, source);
  ut_x11_buffer_append_card32(request, destination);
  ut_x11_buffer_append_card16(request, left);
  ut_x11_buffer_append_card16(request, right);
  ut_x11_buffer_append_card16(request, top);
  ut_x11_buffer_append_card16(request, bottom);

  ut_x11_client_send_request(object, self->major_opcode, 28, request);
}

void ut_x11_xfixes_hide_cursor(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(object, self->major_opcode, 29, request);
}

void ut_x11_xfixes_show_cursor(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_xfixes_extension(object));
  UtX11XfixesExtension *self = (UtX11XfixesExtension *)object;

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(object, self->major_opcode, 30, request);
}

bool ut_object_is_x11_xfixes_extension(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
