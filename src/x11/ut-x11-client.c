#include <assert.h>

#include "ut-x11-buffer.h"
#include "ut-x11-client-private.h"
#include "ut-x11-extension.h"
#include "ut.h"

typedef struct _UtX11Client UtX11Client;

typedef struct {
  UtObject object;
  uint16_t sequence_number;
  UtX11ClientDecodeReplyFunction decode_reply_function;
  UtX11ClientHandleErrorFunction handle_error_function;
  UtObject *callback_object;
  UtObject *cancel;
} Request;

static void request_cleanup(UtObject *object) {
  Request *self = (Request *)object;
  ut_object_unref(self->callback_object);
  ut_object_unref(self->cancel);
}

static UtObjectInterface request_object_interface = {
    .type_name = "Request",
    .cleanup = request_cleanup,
    .interfaces = {{NULL, NULL}}};

static UtObject *
request_new(uint16_t sequence_number,
            UtX11ClientDecodeReplyFunction decode_reply_function,
            UtX11ClientHandleErrorFunction handle_error_function,
            UtObject *callback_object, UtObject *cancel) {
  UtObject *object = ut_object_new(sizeof(Request), &request_object_interface);
  Request *self = (Request *)object;
  self->sequence_number = sequence_number;
  self->decode_reply_function = decode_reply_function;
  self->handle_error_function = handle_error_function;
  self->callback_object = callback_object;
  self->cancel = ut_object_ref(cancel);
  return object;
}

typedef struct {
  UtObject object;
  void *callback;
  void *user_data;
} CallbackData;

static UtObjectInterface callback_data_object_interface = {
    .type_name = "CallbackData", .interfaces = {{NULL, NULL}}};

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
  UtX11Client *client;
  char *name;
} QueryExtensionData;

static void query_extension_data_cleanup(UtObject *object) {
  QueryExtensionData *self = (QueryExtensionData *)object;
  free(self->name);
}

static UtObjectInterface query_extension_data_object_interface = {
    .cleanup = query_extension_data_cleanup,
    .type_name = "QueryExtensionData",
    .interfaces = {{NULL, NULL}}};

static UtObject *query_extension_data_new(UtX11Client *client,
                                          const char *name) {
  UtObject *object = ut_object_new(sizeof(QueryExtensionData),
                                   &query_extension_data_object_interface);
  QueryExtensionData *self = (QueryExtensionData *)object;
  self->client = client;
  self->name = ut_cstring_new(name);
  return object;
}

typedef enum {
  WINDOW_CLASS_INHERIT_FROM_PARENT = 0,
  WINDOW_CLASS_INPUT_OUTPUT = 1,
  WINDOW_CLASS_INPUT_ONLY = 2
} UtX11WindowClass;

typedef enum {
  VALUE_MASK_BACKGROUND_PIXMAP = 0x00000001,
  VALUE_MASK_BACKGROUND_PIXEL = 0x00000002,
  VALUE_MASK_BORDER_PIXMAP = 0x00000004,
  VALUE_MASK_BORDER_PIXEL = 0x00000008,
  VALUE_MASK_BIT_GRAVITY = 0x00000010,
  VALUE_MASK_WIN_GRAVITY = 0x00000020,
  VALUE_MASK_BACKING_STORE = 0x00000040,
  VALUE_MASK_BACKING_PLANES = 0x00000080,
  VALUE_MASK_BACKING_PIXEL = 0x00000100,
  VALUE_MASK_OVERRIDE_REDIRECT = 0x00000200,
  VALUE_MASK_SAVE_UNDER = 0x00000400,
  VALUE_MASK_EVENT_MASK = 0x00000800,
  VALUE_MASK_DO_NOT_PROPAGATE_MASK = 0x00001000,
  VALUE_MASK_COLORMAP = 0x00002000,
  VALUE_MASK_CURSOR = 0x00004000
} UtX11ValueMask;

typedef struct {
  uint8_t depth;
  uint8_t bits_per_pixel;
  uint8_t scanline_pad;
} X11PixmapFormat;

typedef struct {
  uint32_t id;
  uint8_t depth;
  uint8_t class;
  uint8_t bits_per_rgb_value;
  uint16_t colormap_entries;
  uint32_t red_mask;
  uint32_t green_mask;
  uint32_t blue_mask;
} X11Visual;

typedef struct {
  uint32_t root;
  uint32_t default_colormap;
  uint32_t white_pixel;
  uint32_t black_pixel;
  uint32_t current_input_masks;
  uint16_t width_in_pixels;
  uint16_t height_in_pixels;
  uint16_t width_in_millimeters;
  uint16_t height_in_millimeters;
  X11Visual *root_visual;
  X11Visual **visuals;
  size_t visuals_length;
} X11Screen;

struct _UtX11Client {
  UtObject object;
  UtObject *cancel;
  UtObject *socket;
  UtObject *read_cancel;

  UtObject *extensions;
  UtObject *xfixes_extension;
  UtObject *mit_shm_extension;
  UtObject *shape_extension;
  UtObject *present_extension;

  const UtX11EventCallbacks *event_callbacks;
  const UtX11XfixesEventCallbacks *xfixes_event_callbacks;
  const UtX11PresentEventCallbacks *present_event_callbacks;
  UtX11ClientErrorCallback error_callback;
  void *callback_user_data;
  UtObject *callback_cancel;

  UtX11ClientConnectCallback connect_callback;
  void *connect_user_data;
  UtObject *connect_cancel;

  bool setup_complete;
  char *vendor;
  uint32_t release_number;
  uint32_t resource_id_base;
  uint32_t resource_id_mask;
  uint16_t maximum_request_length;
  X11PixmapFormat **pixmap_formats;
  size_t pixmap_formats_length;
  X11Screen **screens;
  size_t screens_length;

  uint32_t next_resource_id;
  uint16_t sequence_number;
  UtObject *requests;
};

static Request *find_request(UtX11Client *self, uint16_t sequence_number) {
  size_t requests_length = ut_list_get_length(self->requests);
  for (size_t i = 0; i < requests_length; i++) {
    Request *request = (Request *)ut_object_list_get_element(self->requests, i);
    if (request->sequence_number == sequence_number) {
      return request;
    }
  }

  return NULL;
}

static void decode_generic_event_enable_reply(UtObject *object, uint8_t data0,
                                              UtObject *data) {
  size_t offset = 0;
  /*uint16_t major_version = */ ut_x11_buffer_get_card16(data, &offset);
  /*uint16_t minor_version = */ ut_x11_buffer_get_card16(data, &offset);
}

static void handle_generic_event_enable_error(UtObject *object,
                                              UtObject *error) {}

static void decode_big_requests_enable_reply(UtObject *object, uint8_t data0,
                                             UtObject *data) {
  UtX11Client *self = (UtX11Client *)object;

  size_t offset = 0;
  uint32_t maximum_request_length = ut_x11_buffer_get_card32(data, &offset);

  self->maximum_request_length = maximum_request_length;
}

static void handle_big_requests_enable_error(UtObject *object,
                                             UtObject *error) {}

static void xfixes_query_version_cb(void *user_data, uint32_t version_major,
                                    uint32_t version_minor, UtObject *error) {}

static void mit_shm_query_version_cb(void *user_data, uint16_t major_version,
                                     uint16_t minor_version, uint16_t uid,
                                     uint16_t gid, uint8_t pixmap_format,
                                     bool shared_pixmaps, UtObject *error) {}

static void shape_query_version_cb(void *user_data, uint16_t major_version,
                                   uint16_t minor_version, UtObject *error) {}

static void present_query_version_cb(void *user_data, uint32_t version_major,
                                     uint32_t version_minor, UtObject *error) {}

static void decode_query_extension_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  QueryExtensionData *query_extension_data = (QueryExtensionData *)object;
  UtX11Client *self = query_extension_data->client;

  size_t offset = 0;
  bool present = ut_x11_buffer_get_bool(data, &offset);
  uint8_t major_opcode = ut_x11_buffer_get_card8(data, &offset);
  uint8_t first_event = ut_x11_buffer_get_card8(data, &offset);
  uint8_t first_error = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (present) {
    if (ut_cstring_equal(query_extension_data->name,
                         "Generic Event Extension")) {
      ut_x11_client_send_request_with_reply(
          (UtObject *)self, major_opcode, 0, NULL,
          decode_generic_event_enable_reply, handle_generic_event_enable_error,
          (void *)self, self->cancel);
    } else if (ut_cstring_equal(query_extension_data->name, "BIG-REQUESTS")) {
      ut_x11_client_send_request_with_reply(
          (UtObject *)self, major_opcode, 0, NULL,
          decode_big_requests_enable_reply, handle_big_requests_enable_error,
          (void *)self, self->cancel);
    } else if (ut_cstring_equal(query_extension_data->name, "XFIXES")) {
      self->xfixes_extension = ut_x11_xfixes_extension_new(
          (UtObject *)self, major_opcode, first_event, first_error,
          self->xfixes_event_callbacks, self->callback_user_data,
          self->callback_cancel);
      ut_list_append(self->extensions, self->xfixes_extension);

      ut_x11_xfixes_extension_query_version(
          self->xfixes_extension, xfixes_query_version_cb, self, self->cancel);
    } else if (ut_cstring_equal(query_extension_data->name, "MIT-SHM")) {
      self->mit_shm_extension = ut_x11_mit_shm_extension_new(
          (UtObject *)self, major_opcode, first_event, first_error);
      ut_list_append(self->extensions, self->mit_shm_extension);

      ut_x11_mit_shm_extension_query_version(self->mit_shm_extension,
                                             mit_shm_query_version_cb, self,
                                             self->cancel);
    } else if (ut_cstring_equal(query_extension_data->name, "SHAPE")) {
      self->shape_extension = ut_x11_shape_extension_new(
          (UtObject *)self, major_opcode, first_event, NULL, NULL,
          self->callback_cancel);
      ut_list_append(self->extensions, self->shape_extension);

      ut_x11_shape_extension_query_version(
          self->shape_extension, shape_query_version_cb, self, self->cancel);
    } else if (ut_cstring_equal(query_extension_data->name, "Present")) {
      self->present_extension = ut_x11_present_extension_new(
          (UtObject *)self, major_opcode, self->present_event_callbacks,
          self->callback_user_data, self->callback_cancel);
      ut_list_append(self->extensions, self->present_extension);

      ut_x11_present_extension_query_version(self->present_extension,
                                             present_query_version_cb, self,
                                             self->cancel);

      // FIXME: More reliably do this on the last setup request.
      self->connect_callback(self->connect_user_data, NULL);
    }
  }
}

static void handle_query_extension_error(UtObject *object, UtObject *error) {
  // FIXME: Connection error
}

static void query_extension(UtX11Client *self, const char *name) {
  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request_with_reply(
      (UtObject *)self, 98, 0, request, decode_query_extension_reply,
      handle_query_extension_error, query_extension_data_new(self, name),
      NULL); // FIXME: Cancel
}

static void send_request(UtObject *object, uint8_t opcode, uint8_t data0,
                         UtObject *data,
                         UtX11ClientDecodeReplyFunction decode_reply_function,
                         UtX11ClientHandleErrorFunction handle_error_function,
                         UtObject *callback_object, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;

  size_t data_length = data != NULL ? ut_list_get_length(data) : 0;
  assert(data_length % 4 == 0);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card8(request, opcode);
  ut_x11_buffer_append_card8(request, data0);
  ut_x11_buffer_append_card16(request, 1 + data_length / 4);
  if (data != NULL) {
    ut_list_append_list(ut_x11_buffer_get_data(request),
                        ut_x11_buffer_get_data(data));
    ut_list_append_list(ut_x11_buffer_get_fds(request),
                        ut_x11_buffer_get_fds(data));
  }

  self->sequence_number++;

  if (decode_reply_function != NULL) {
    UtObjectRef request =
        request_new(self->sequence_number, decode_reply_function,
                    handle_error_function, callback_object, cancel);
    ut_list_append(self->requests, request);
  }

  ut_output_stream_write(self->socket, request);
}

static size_t decode_setup_failed(UtX11Client *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 8) {
    return 0;
  }

  size_t offset = 0;
  assert(ut_x11_buffer_get_card8(data, &offset) == 0);
  uint8_t reason_length = ut_x11_buffer_get_card8(data, &offset);
  /*uint16_t protocol_major_version = */ ut_x11_buffer_get_card16(data,
                                                                  &offset);
  /*uint16_t protocol_minor_version = */ ut_x11_buffer_get_card16(data,
                                                                  &offset);
  uint16_t length = ut_x11_buffer_get_card16(data, &offset);
  size_t message_length = (length + 2) * 4;
  if (data_length < message_length) {
    return 0;
  }
  ut_cstring_ref reason =
      ut_x11_buffer_get_string8(data, &offset, reason_length);

  UtObjectRef error =
      ut_general_error_new("Failed to connect to X server: %s", reason);
  self->connect_callback(self->connect_user_data, error);

  return offset;
}

static size_t decode_setup_success(UtX11Client *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 8) {
    return 0;
  }

  size_t offset = 0;
  assert(ut_x11_buffer_get_card8(data, &offset) == 1);
  ut_x11_buffer_get_padding(data, &offset, 1);
  /*uint16_t protocol_major_version = */ ut_x11_buffer_get_card16(data,
                                                                  &offset);
  /*uint16_t protocol_minor_version = */ ut_x11_buffer_get_card16(data,
                                                                  &offset);
  uint16_t length = ut_x11_buffer_get_card16(data, &offset);
  size_t message_length = (length + 2) * 4;
  if (data_length < message_length) {
    return 0;
  }
  self->release_number = ut_x11_buffer_get_card32(data, &offset);
  self->resource_id_base = ut_x11_buffer_get_card32(data, &offset);
  self->resource_id_mask = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // motion_buffer_size
  uint16_t vendor_length = ut_x11_buffer_get_card16(data, &offset);
  self->maximum_request_length = ut_x11_buffer_get_card16(data, &offset);
  size_t screens_length = ut_x11_buffer_get_card8(data, &offset);
  size_t pixmap_formats_length = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card8(data, &offset); // image_byte_order
  ut_x11_buffer_get_card8(data, &offset); // bitmap_format_bit_order
  ut_x11_buffer_get_card8(data, &offset); // bitmap_format_scanline_unit
  ut_x11_buffer_get_card8(data, &offset); // bitmap_format_scanline_pad
  ut_x11_buffer_get_card8(data, &offset); // min_keycode
  ut_x11_buffer_get_card8(data, &offset); // max_keycode
  ut_x11_buffer_get_padding(data, &offset, 4);
  self->vendor = ut_x11_buffer_get_string8(data, &offset, vendor_length);
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  self->pixmap_formats =
      malloc(sizeof(X11PixmapFormat *) * pixmap_formats_length);
  self->pixmap_formats_length = pixmap_formats_length;
  for (size_t i = 0; i < pixmap_formats_length; i++) {
    X11PixmapFormat *format = self->pixmap_formats[i] =
        malloc(sizeof(X11PixmapFormat));
    format->depth = ut_x11_buffer_get_card8(data, &offset); // depth
    format->bits_per_pixel =
        ut_x11_buffer_get_card8(data, &offset); // bits_per_pixel
    format->scanline_pad =
        ut_x11_buffer_get_card8(data, &offset); // scanline_pad
    ut_x11_buffer_get_padding(data, &offset, 5);
  }

  self->screens = malloc(sizeof(X11Screen *) * screens_length);
  self->screens_length = screens_length;
  for (size_t i = 0; i < screens_length; i++) {
    X11Screen *screen = self->screens[i] = malloc(sizeof(X11Screen));

    screen->root = ut_x11_buffer_get_card32(data, &offset);
    screen->default_colormap = ut_x11_buffer_get_card32(data, &offset);
    screen->white_pixel = ut_x11_buffer_get_card32(data, &offset);
    screen->black_pixel = ut_x11_buffer_get_card32(data, &offset);
    screen->current_input_masks = ut_x11_buffer_get_card32(data, &offset);
    screen->width_in_pixels = ut_x11_buffer_get_card16(data, &offset);
    screen->height_in_pixels = ut_x11_buffer_get_card16(data, &offset);
    screen->width_in_millimeters = ut_x11_buffer_get_card16(data, &offset);
    screen->height_in_millimeters = ut_x11_buffer_get_card16(data, &offset);
    ut_x11_buffer_get_card16(data, &offset); // min_installed_maps
    ut_x11_buffer_get_card16(data, &offset); // max_installed_maps
    screen->root_visual = NULL;
    uint32_t root_visual_id = ut_x11_buffer_get_card32(data, &offset);
    ut_x11_buffer_get_card8(data, &offset); // backing_stores
    ut_x11_buffer_get_card8(data, &offset); // save_unders
    ut_x11_buffer_get_card8(data, &offset); // root_depth
    screen->visuals = NULL;
    screen->visuals_length = 0;
    size_t allowed_depths_length = ut_x11_buffer_get_card8(data, &offset);
    for (size_t j = 0; j < allowed_depths_length; j++) {
      uint8_t depth = ut_x11_buffer_get_card8(data, &offset);
      ut_x11_buffer_get_padding(data, &offset, 1);
      size_t visuals_length = ut_x11_buffer_get_card16(data, &offset);
      ut_x11_buffer_get_padding(data, &offset, 4);
      size_t first_visual = screen->visuals_length;
      screen->visuals_length += visuals_length;
      screen->visuals = realloc(screen->visuals,
                                sizeof(X11Visual *) * screen->visuals_length);
      for (size_t k = 0; k < visuals_length; k++) {
        X11Visual *visual = screen->visuals[first_visual + k] =
            malloc(sizeof(X11Visual));
        visual->id = ut_x11_buffer_get_card32(data, &offset);
        visual->depth = depth;
        visual->class = ut_x11_buffer_get_card8(data, &offset);
        visual->bits_per_rgb_value = ut_x11_buffer_get_card8(data, &offset);
        visual->colormap_entries = ut_x11_buffer_get_card16(data, &offset);
        visual->red_mask = ut_x11_buffer_get_card32(data, &offset);
        visual->blue_mask = ut_x11_buffer_get_card32(data, &offset);
        visual->green_mask = ut_x11_buffer_get_card32(data, &offset);
        ut_x11_buffer_get_padding(data, &offset, 4);

        if (visual->id == root_visual_id) {
          screen->root_visual = visual;
        }
      }
    }
  }

  self->setup_complete = true;

  query_extension(self, "Generic Event Extension");
  query_extension(self, "BIG-REQUESTS");
  query_extension(self, "XFIXES");
  query_extension(self, "MIT-SHM");
  query_extension(self, "SHAPE");
  query_extension(self, "Present");

  return offset;
}

static size_t decode_setup_authenticate(UtObject *message) { return 0; }

static size_t decode_setup_message(UtX11Client *self, UtObject *message) {
  uint8_t status = ut_uint8_list_get_element(message, 0);
  if (status == 0) {
    return decode_setup_failed(self, message);
  } else if (status == 1) {
    return decode_setup_success(self, message);
  } else if (status == 2) {
    return decode_setup_authenticate(message);
  } else {
    assert(false);
  }
}

static size_t decode_error(UtX11Client *self, UtObject *data) {
  if (ut_list_get_length(data) < 32) {
    return false;
  }

  UtObjectRef error_data = ut_list_get_sublist(data, 0, 32);

  size_t offset = 0;
  assert(ut_x11_buffer_get_card8(error_data, &offset) == 0);
  uint8_t code = ut_x11_buffer_get_card8(error_data, &offset);
  uint16_t sequence_number = ut_x11_buffer_get_card16(error_data, &offset);
  uint32_t value = ut_x11_buffer_get_card32(error_data, &offset);
  uint16_t minor_opcode = ut_x11_buffer_get_card16(error_data, &offset);
  uint8_t major_opcode = ut_x11_buffer_get_card8(error_data, &offset);

  UtObjectRef error = NULL;
  if (code == 1) {
    error = ut_x11_request_error_new();
  } else if (code == 2) {
    error = ut_x11_value_error_new(value);
  } else if (code == 3) {
    error = ut_x11_window_error_new(value);
  } else if (code == 4) {
    error = ut_x11_pixmap_error_new(value);
  } else if (code == 5) {
    error = ut_x11_atom_error_new(value);
  } else if (code == 8) {
    error = ut_x11_match_error_new();
  } else if (code == 9) {
    error = ut_x11_drawable_error_new(value);
  } else if (code == 10) {
    error = ut_x11_access_error_new();
  } else if (code == 11) {
    error = ut_x11_alloc_error_new();
  } else if (code == 12) {
    error = ut_x11_colormap_error_new(value);
  } else if (code == 13) {
    error = ut_x11_gcontext_error_new(value);
  } else if (code == 14) {
    error = ut_x11_id_choice_error_new(value);
  } else if (code == 15) {
    error = ut_x11_name_error_new();
  } else if (code == 16) {
    error = ut_x11_length_error_new(sequence_number);
  } else if (code == 17) {
    error = ut_x11_implementation_error_new();
  } else {
    size_t extensions_length = ut_list_get_length(self->extensions);
    for (size_t i = 0; i < extensions_length; i++) {
      UtObject *extension = ut_object_list_get_element(self->extensions, i);
      error = ut_x11_extension_decode_error(extension, error_data);
      if (error != NULL) {
        break;
      }
    }
    if (error == NULL) {
      error = ut_x11_unknown_error_new(code, major_opcode, minor_opcode);
    }
  }

  Request *request = find_request(self, sequence_number);
  if (request != NULL) {
    if (!ut_cancel_is_active(request->cancel)) {
      request->handle_error_function(request->callback_object, error);
    }
  } else if (self->error_callback != NULL) {
    self->error_callback(self->callback_user_data, error);
  }

  return 32;
}

static void decode_intern_atom_reply(UtObject *object, uint8_t data0,
                                     UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint32_t atom = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 20);

  if (callback_data->callback != NULL) {
    UtX11InternAtomCallback callback =
        (UtX11InternAtomCallback)callback_data->callback;
    callback(callback_data->user_data, atom, NULL);
  }
}

static void handle_intern_atom_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11InternAtomCallback callback =
        (UtX11InternAtomCallback)callback_data->callback;
    callback(callback_data->user_data, 0, error);
  }
}

static void decode_get_atom_name_reply(UtObject *object, uint8_t data0,
                                       UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint16_t name_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);
  ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback != NULL) {
    UtX11GetAtomNameCallback callback =
        (UtX11GetAtomNameCallback)callback_data->callback;
    callback(callback_data->user_data, name, NULL);
  }
}

static void handle_get_atom_name_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11GetAtomNameCallback callback =
        (UtX11GetAtomNameCallback)callback_data->callback;
    callback(callback_data->user_data, NULL, error);
  }
}

static void decode_get_property_reply(UtObject *object, uint8_t data0,
                                      UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  uint8_t format = data0;
  uint32_t type = ut_x11_buffer_get_card32(data, &offset);
  uint32_t bytes_after = ut_x11_buffer_get_card32(data, &offset);
  size_t length = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 12);
  UtObjectRef value = NULL;
  if (format == 0) {
  } else if (format == 8) {
    value = ut_uint8_array_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint8_list_append(value, ut_x11_buffer_get_card8(data, &offset));
    }
  } else if (format == 16) {
    value = ut_uint16_list_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint16_list_append(value, ut_x11_buffer_get_card16(data, &offset));
    }
  } else if (format == 32) {
    value = ut_uint32_list_new();
    for (size_t i = 0; i < length; i++) {
      ut_uint32_list_append(value, ut_x11_buffer_get_card32(data, &offset));
    }
  } else {
    assert(false);
  }
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback != NULL) {
    UtX11GetPropertyCallback callback =
        (UtX11GetPropertyCallback)callback_data->callback;
    callback(callback_data->user_data, type, value, bytes_after, NULL);
  }
}

static void handle_get_property_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11GetPropertyCallback callback =
        (UtX11GetPropertyCallback)callback_data->callback;
    callback(callback_data->user_data, 0, NULL, 0, error);
  }
}

static void decode_list_properties_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  size_t atoms_length = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 22);
  UtObjectRef atoms = ut_uint32_list_new();
  for (size_t i = 0; i < atoms_length; i++) {
    ut_uint32_list_append(atoms, ut_x11_buffer_get_card32(data, &offset));
  }

  if (callback_data->callback != NULL) {
    UtX11ListPropertiesCallback callback =
        (UtX11ListPropertiesCallback)callback_data->callback;
    callback(callback_data->user_data, atoms, NULL);
  }
}

static void handle_list_properties_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ListPropertiesCallback callback =
        (UtX11ListPropertiesCallback)callback_data->callback;
    callback(callback_data->user_data, NULL, error);
  }
}

static void decode_list_extensions_reply(UtObject *object, uint8_t data0,
                                         UtObject *data) {
  CallbackData *callback_data = (CallbackData *)object;

  size_t offset = 0;
  ut_x11_buffer_get_padding(data, &offset, 24);
  size_t names_length = data0;
  UtObjectRef names = ut_string_list_new();
  for (size_t i = 0; i < names_length; i++) {
    uint8_t name_length = ut_x11_buffer_get_card8(data, &offset);
    ut_cstring_ref name = ut_x11_buffer_get_string8(data, &offset, name_length);
    ut_string_list_append(names, name);
  }
  ut_x11_buffer_get_align_padding(data, &offset, 4);

  if (callback_data->callback != NULL) {
    UtX11ListExtensionsCallback callback =
        (UtX11ListExtensionsCallback)callback_data->callback;
    callback(callback_data->user_data, names, NULL);
  }
}

static void handle_list_extensions_error(UtObject *object, UtObject *error) {
  CallbackData *callback_data = (CallbackData *)object;

  if (callback_data->callback != NULL) {
    UtX11ListExtensionsCallback callback =
        (UtX11ListExtensionsCallback)callback_data->callback;
    callback(callback_data->user_data, NULL, error);
  }
}

static size_t decode_reply(UtX11Client *self, UtObject *data) {
  size_t data_length = ut_list_get_length(data);
  if (data_length < 4) {
    return 0;
  }

  size_t offset = 0;
  assert(ut_x11_buffer_get_card8(data, &offset) == 1);
  uint8_t data0 = ut_x11_buffer_get_card8(data, &offset);
  uint16_t sequence_number = ut_x11_buffer_get_card16(data, &offset);
  uint32_t length = ut_x11_buffer_get_card32(data, &offset);

  size_t payload_length = 32 + length * 4;
  if (data_length < payload_length) {
    return 0;
  }

  Request *request = find_request(self, sequence_number);
  if (request != NULL) {
    UtObjectRef payload =
        ut_list_get_sublist(data, offset, payload_length - offset);
    if (!ut_cancel_is_active(request->cancel)) {
      request->decode_reply_function(request->callback_object, data0, payload);
    }
  } else {
    // FIXME: Warn about unexpected reply.
  }

  return payload_length;
}

static void decode_key_press(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 2);
  uint8_t keycode = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // same_screen
  ut_x11_buffer_get_padding(data, &offset, 1);

  if (self->event_callbacks->key_press != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->key_press(self->callback_user_data, window, keycode,
                                     x, y);
  }
}

static void decode_key_release(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 3);
  uint8_t keycode = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // same_screen
  ut_x11_buffer_get_padding(data, &offset, 1);

  if (self->event_callbacks->key_release != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->key_release(self->callback_user_data, window,
                                       keycode, x, y);
  }
}

static void decode_button_press(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 4);
  uint8_t button = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // same_screen
  ut_x11_buffer_get_padding(data, &offset, 1);

  if (self->event_callbacks->button_press != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->button_press(self->callback_user_data, window,
                                        button, x, y);
  }
}

static void decode_button_release(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 5);
  uint8_t button = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // same_screen
  ut_x11_buffer_get_padding(data, &offset, 1);

  if (self->event_callbacks->button_release != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->button_release(self->callback_user_data, window,
                                          button, x, y);
  }
}

static void decode_motion_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 6);
  ut_x11_buffer_get_card8(data, &offset);  // detail
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // same_screen
  ut_x11_buffer_get_padding(data, &offset, 1);

  if (self->event_callbacks->motion_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->motion_notify(self->callback_user_data, window, x,
                                         y);
  }
}

static void decode_enter_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 7);
  ut_x11_buffer_get_card8(data, &offset);  // detail
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // mode
  ut_x11_buffer_get_card8(data, &offset);  // same_screen, focus

  if (self->event_callbacks->enter_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->enter_notify(self->callback_user_data, window, x, y);
  }
}

static void decode_leave_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 8);
  ut_x11_buffer_get_card8(data, &offset);  // detail
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card32(data, &offset); // root
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // child
  ut_x11_buffer_get_int16(data, &offset);  // root_x
  ut_x11_buffer_get_int16(data, &offset);  // root_y
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // state
  ut_x11_buffer_get_card8(data, &offset);  // mode
  ut_x11_buffer_get_card8(data, &offset);  // same_screen, focus

  if (self->event_callbacks->leave_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->leave_notify(self->callback_user_data, window, x, y);
  }
}

static void decode_focus_in(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 9);
  ut_x11_buffer_get_card8(data, &offset);  // detail
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card8(data, &offset); // mode

  if (self->event_callbacks->focus_in != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->focus_in(self->callback_user_data, window);
  }
}

static void decode_focus_out(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 10);
  ut_x11_buffer_get_card8(data, &offset);  // detail
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card8(data, &offset); // mode

  if (self->event_callbacks->focus_out != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->focus_out(self->callback_user_data, window);
  }
}

static void decode_expose(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 12);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint16_t x = ut_x11_buffer_get_card16(data, &offset);
  uint16_t y = ut_x11_buffer_get_card16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // count
  ut_x11_buffer_get_padding(data, &offset, 14);

  if (self->event_callbacks->expose != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->expose(self->callback_user_data, window, x, y, width,
                                  height);
  }
}

static void decode_no_expose(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 14);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t drawable = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // minor-opcode
  ut_x11_buffer_get_card8(data, &offset);  // major-opcode

  if (self->event_callbacks->no_expose != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->no_expose(self->callback_user_data, drawable);
  }
}

static void decode_map_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 19);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t event = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  bool override_redirect = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 19);

  if (self->event_callbacks->map_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->map_notify(self->callback_user_data, event, window,
                                      override_redirect);
  }
}

static void decode_reparent_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 21);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t event = ut_x11_buffer_get_card32(data, &offset);
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t parent = ut_x11_buffer_get_card32(data, &offset);
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  bool override_redirect = ut_x11_buffer_get_bool(data, &offset);
  ut_x11_buffer_get_padding(data, &offset, 11);

  if (self->event_callbacks->reparent_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->reparent_notify(self->callback_user_data, event,
                                           window, parent, x, y,
                                           override_redirect);
  }
}

static void decode_configure_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 22);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  ut_x11_buffer_get_card32(data, &offset); // event
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // above_sibling
  int16_t x = ut_x11_buffer_get_int16(data, &offset);
  int16_t y = ut_x11_buffer_get_int16(data, &offset);
  uint16_t width = ut_x11_buffer_get_card16(data, &offset);
  uint16_t height = ut_x11_buffer_get_card16(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // border_width
  ut_x11_buffer_get_card8(data, &offset);  // override_redirect
  ut_x11_buffer_get_padding(data, &offset, 5);

  if (self->event_callbacks->configure_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->configure_notify(self->callback_user_data, window, x,
                                            y, width, height);
  }
}

static void decode_property_notify(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 28);
  ut_x11_buffer_get_padding(data, &offset, 1);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t window = ut_x11_buffer_get_card32(data, &offset);
  uint32_t atom = ut_x11_buffer_get_card32(data, &offset);
  ut_x11_buffer_get_card32(data, &offset); // time
  ut_x11_buffer_get_card8(data, &offset);  // state
  ut_x11_buffer_get_padding(data, &offset, 15);

  if (self->event_callbacks->property_notify != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->property_notify(self->callback_user_data, window,
                                           atom);
  }
}

static size_t decode_generic_event(UtX11Client *self, UtObject *data) {
  size_t offset = 0;
  assert((ut_x11_buffer_get_card8(data, &offset) & 0x7f) == 35);
  uint8_t major_opcode = ut_x11_buffer_get_card8(data, &offset);
  ut_x11_buffer_get_card16(data, &offset); // sequence_number
  uint32_t extra_length = ut_x11_buffer_get_card32(data, &offset);
  uint16_t code = ut_x11_buffer_get_card16(data, &offset);

  size_t total_length = 32 + extra_length * 4;
  if (ut_list_get_length(data) < total_length) {
    return 0;
  }
  UtObjectRef event_data = ut_list_get_sublist(data, 10, total_length);

  size_t extensions_length = ut_list_get_length(self->extensions);
  for (size_t i = 0; i < extensions_length; i++) {
    UtObject *extension = ut_object_list_get_element(self->extensions, i);
    // FIXME: Major opcode doesn't need passing - we know which extension this
    // is for
    if (ut_x11_extension_decode_generic_event(extension, major_opcode, code,
                                              event_data)) {
      return total_length;
    }
  }

  if (self->event_callbacks->unknown_generic_event != NULL &&
      !ut_cancel_is_active(self->callback_cancel)) {
    self->event_callbacks->unknown_generic_event(self->callback_user_data,
                                                 major_opcode, code);
  }

  return total_length;
}

static bool decode_extension_event(UtX11Client *self, UtObject *data) {
  size_t extensions_length = ut_list_get_length(self->extensions);
  for (size_t i = 0; i < extensions_length; i++) {
    UtObject *extension = ut_object_list_get_element(self->extensions, i);
    if (ut_x11_extension_decode_event(extension, data)) {
      return true;
    }
  }

  return false;
}

static size_t decode_event(UtX11Client *self, UtObject *data) {
  if (ut_list_get_length(data) < 32) {
    return 0;
  }

  UtObjectRef event_data = ut_list_get_sublist(data, 0, 32);

  uint8_t code = ut_uint8_list_get_element(event_data, 0);
  // bool from_send_event = (code & 0x80) != 0;
  code &= 0x7f;
  switch (code) {
  case 2:
    decode_key_press(self, event_data);
    break;
  case 3:
    decode_key_release(self, event_data);
    break;
  case 4:
    decode_button_press(self, event_data);
    break;
  case 5:
    decode_button_release(self, event_data);
    break;
  case 6:
    decode_motion_notify(self, event_data);
    break;
  case 7:
    decode_enter_notify(self, event_data);
    break;
  case 8:
    decode_leave_notify(self, event_data);
    break;
  case 9:
    decode_focus_in(self, event_data);
    break;
  case 10:
    decode_focus_out(self, event_data);
    break;
  case 12:
    decode_expose(self, event_data);
    break;
  case 14:
    decode_no_expose(self, event_data);
    break;
  case 19:
    decode_map_notify(self, event_data);
    break;
  case 21:
    decode_reparent_notify(self, event_data);
    break;
  case 22:
    decode_configure_notify(self, event_data);
    break;
  case 28:
    decode_property_notify(self, event_data);
    break;
  case 35:
    return decode_generic_event(self, data);
  default:
    if (!decode_extension_event(self, data)) {
      if (self->event_callbacks->unknown_event != NULL &&
          !ut_cancel_is_active(self->callback_cancel)) {
        self->event_callbacks->unknown_event(self->callback_user_data, code);
      }
    }
  }

  return 32;
}

static size_t decode_message(UtX11Client *self, UtObject *data) {
  uint8_t code = ut_uint8_list_get_element(data, 0);
  if (code == 0) {
    return decode_error(self, data);
  } else if (code == 1) {
    return decode_reply(self, data);
  } else {
    return decode_event(self, data);
  }
}

static size_t read_cb(void *user_data, UtObject *data, bool complete) {
  UtX11Client *self = user_data;

  size_t offset = 0;
  if (!self->setup_complete) {
    UtObjectRef buffer = ut_x11_buffer_new_from_data(data);
    offset = decode_setup_message(self, buffer);
    if (offset == 0) {
      return 0;
    }
  }

  size_t data_length = ut_list_get_length(data);
  while (offset < data_length) {
    UtObjectRef message =
        ut_list_get_sublist(data, offset, data_length - offset);
    UtObjectRef buffer = ut_x11_buffer_new_from_data(message);
    size_t n_used = decode_message(self, buffer);
    if (n_used == 0) {
      return offset;
    }
    offset += n_used;
  }

  if (complete) {
    ut_cancel_activate(self->read_cancel);
  }

  return offset;
}

static void connect_cb(void *user_data) {
  UtX11Client *self = user_data;

  UtObjectRef setup = ut_x11_buffer_new();
  ut_x11_buffer_append_card8(setup, 0x6c); // Little endian.
  ut_x11_buffer_append_padding(setup, 1);
  ut_x11_buffer_append_card16(setup, 11); // Protocol major version.
  ut_x11_buffer_append_card16(setup, 0);  // Protocol minor version.
  ut_x11_buffer_append_card16(setup, 0);  // Authorizaton protocol name length.
  ut_x11_buffer_append_card16(setup, 0);  // Authorizaton protocol data length.
  ut_x11_buffer_append_padding(setup, 2);
  // Authorization protocol name.
  ut_x11_buffer_append_align_padding(setup, 4);
  // Authorization protocol data.
  ut_x11_buffer_append_align_padding(setup, 4);

  ut_output_stream_write(self->socket, setup);
}

static void ut_x11_client_init(UtObject *object) {
  UtX11Client *self = (UtX11Client *)object;
  self->cancel = ut_cancel_new();
  self->read_cancel = ut_cancel_new();
  self->extensions = ut_object_list_new();
  self->requests = ut_object_list_new();
}

static void ut_x11_client_cleanup(UtObject *object) {
  UtX11Client *self = (UtX11Client *)object;

  ut_cancel_activate(self->cancel);
  ut_cancel_activate(self->read_cancel);

  size_t extensions_length = ut_list_get_length(self->extensions);
  for (size_t i = 0; i < extensions_length; i++) {
    UtObject *extension = ut_object_list_get_element(self->extensions, i);
    ut_x11_extension_close(extension);
  }

  ut_object_unref(self->cancel);
  ut_object_unref(self->socket);
  ut_object_unref(self->read_cancel);
  ut_object_unref(self->extensions);
  ut_object_unref(self->mit_shm_extension);
  ut_object_unref(self->present_extension);
  ut_object_unref(self->callback_cancel);
  ut_object_unref(self->connect_cancel);
  free(self->vendor);
  for (size_t i = 0; i < self->pixmap_formats_length; i++) {
    free(self->pixmap_formats[i]);
  }
  free(self->pixmap_formats);
  for (size_t i = 0; i < self->screens_length; i++) {
    X11Screen *screen = self->screens[i];
    for (size_t j = 0; j < screen->visuals_length; j++) {
      free(screen->visuals[j]);
    }
    free(screen);
  }
  free(self->screens);
  ut_object_unref(self->requests);
}

static UtObjectInterface object_interface = {.type_name = "UtX11Client",
                                             .init = ut_x11_client_init,
                                             .cleanup = ut_x11_client_cleanup,
                                             .interfaces = {{NULL, NULL}}};

UtObject *
ut_x11_client_new(const UtX11EventCallbacks *event_callbacks,
                  const UtX11XfixesEventCallbacks *xfixes_event_callbacks,
                  const UtX11PresentEventCallbacks *present_event_callbacks,
                  UtX11ClientErrorCallback error_callback, void *user_data,
                  UtObject *cancel) {
  UtObject *object = ut_object_new(sizeof(UtX11Client), &object_interface);
  UtX11Client *self = (UtX11Client *)object;
  UtObjectRef address = ut_unix_socket_address_new("/tmp/.X11-unix/X0");
  self->socket = ut_tcp_socket_new(address, 0);
  self->event_callbacks = event_callbacks;
  self->present_event_callbacks = present_event_callbacks;
  self->error_callback = error_callback;
  self->callback_user_data = user_data;
  self->callback_cancel = ut_object_ref(cancel);
  return object;
}

void ut_x11_client_connect(UtObject *object,
                           UtX11ClientConnectCallback callback, void *user_data,
                           UtObject *cancel) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;

  assert(callback != NULL);

  assert(self->connect_callback == NULL);
  self->connect_callback = callback;
  self->connect_user_data = user_data;
  self->connect_cancel = ut_object_ref(cancel);

  ut_tcp_socket_connect(self->socket, connect_cb, self, self->cancel);
  ut_input_stream_read(self->socket, read_cb, self, self->read_cancel);
}

uint32_t ut_x11_client_create_resource_id(UtObject *object) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;

  return self->resource_id_base | (self->next_resource_id++);
}

void ut_x11_client_send_request(UtObject *object, uint8_t opcode, uint8_t data0,
                                UtObject *data) {
  send_request(object, opcode, data0, data, NULL, NULL, NULL, NULL);
}

void ut_x11_client_send_request_with_reply(
    UtObject *object, uint8_t opcode, uint8_t data0, UtObject *data,
    UtX11ClientDecodeReplyFunction decode_reply_function,
    UtX11ClientHandleErrorFunction handle_error_function,
    UtObject *callback_object, UtObject *cancel) {
  send_request(object, opcode, data0, data, decode_reply_function,
               handle_error_function, callback_object, cancel);
}

uint32_t ut_x11_client_create_window(UtObject *object, int16_t x, int16_t y,
                                     uint16_t width, uint16_t height,
                                     uint32_t event_mask) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;

  uint32_t id = ut_x11_client_create_resource_id(object);

  assert(self->screens_length > 0);
  X11Screen *screen = self->screens[0];

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, screen->root); // parent
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_card16(request, 0); // border_width
  ut_x11_buffer_append_card16(request, WINDOW_CLASS_INPUT_OUTPUT);
  ut_x11_buffer_append_card32(request, screen->root_visual->id);
  ut_x11_buffer_append_card32(request, VALUE_MASK_EVENT_MASK);
  ut_x11_buffer_append_card32(request, event_mask);

  ut_x11_client_send_request(object, 1, screen->root_visual->depth, request);

  return id;
}

void ut_x11_client_destroy_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(object, 4, 0, request);
}

void ut_x11_client_map_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(object, 8, 0, request);
}

void ut_x11_client_unmap_window(UtObject *object, uint32_t window) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request(object, 10, 0, request);
}

void ut_x11_client_configure_window(UtObject *object, uint32_t window,
                                    int16_t x, int16_t y, uint16_t width,
                                    uint16_t height) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card16(request, 0x0001 | 0x0002 | 0x0004 | 0x0008);
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_value_int16(request, x);
  ut_x11_buffer_append_value_int16(request, y);
  ut_x11_buffer_append_value_card16(request, width);
  ut_x11_buffer_append_value_card16(request, height);

  ut_x11_client_send_request(object, 12, 0, request);
}

void ut_x11_client_intern_atom(UtObject *object, const char *name,
                               bool only_if_exists,
                               UtX11InternAtomCallback callback,
                               void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card16(request, ut_cstring_get_length(name));
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_string8(request, name);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request_with_reply(
      object, 16, only_if_exists ? 1 : 0, request, decode_intern_atom_reply,
      handle_intern_atom_error, callback_data_new(callback, user_data), cancel);
}

void ut_x11_client_get_atom_name(UtObject *object, uint32_t atom,
                                 UtX11GetAtomNameCallback callback,
                                 void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, atom);

  ut_x11_client_send_request_with_reply(
      object, 17, 0, request, decode_get_atom_name_reply,
      handle_get_atom_name_error, callback_data_new(callback, user_data),
      cancel);
}

void ut_x11_client_change_property(UtObject *object, uint32_t window,
                                   uint32_t property, uint32_t type) {
  assert(ut_object_is_x11_client(object));
}

void ut_x11_client_delete_property(UtObject *object, uint32_t window,
                                   uint32_t property) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);

  ut_x11_client_send_request(object, 19, 0, request);
}

void ut_x11_client_get_property(UtObject *object, uint32_t window,
                                uint32_t property,
                                UtX11GetPropertyCallback callback,
                                void *user_data, UtObject *cancel) {
  return ut_x11_client_get_property_full(object, window, property, 0, 0,
                                         0xffffffff, false, callback, user_data,
                                         cancel);
}

void ut_x11_client_get_property_full(UtObject *object, uint32_t window,
                                     uint32_t property, uint32_t type,
                                     uint32_t long_offset, uint32_t long_length,
                                     bool delete,
                                     UtX11GetPropertyCallback callback,
                                     void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_card32(request, property);
  ut_x11_buffer_append_card32(request, type);
  ut_x11_buffer_append_card32(request, long_offset);
  ut_x11_buffer_append_card32(request, long_length);

  ut_x11_client_send_request_with_reply(
      object, 31, delete ? 1 : 0, request, decode_get_property_reply,
      handle_get_property_error, callback_data_new(callback, user_data),
      cancel);
}

void ut_x11_client_list_properties(UtObject *object, uint32_t window,
                                   UtX11ListPropertiesCallback callback,
                                   void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);

  ut_x11_client_send_request_with_reply(
      object, 32, 0, request, decode_list_properties_reply,
      handle_list_properties_error, callback_data_new(callback, user_data),
      cancel);
}

uint32_t ut_x11_client_create_pixmap(UtObject *object, uint32_t drawable,
                                     uint16_t width, uint16_t height,
                                     uint8_t depth) {
  assert(ut_object_is_x11_client(object));

  uint32_t id = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(object, 53, depth, request);

  return id;
}

void ut_x11_client_free_pixmap(UtObject *object, uint32_t pixmap) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, pixmap);

  ut_x11_client_send_request(object, 54, 0, request);
}

uint32_t ut_x11_client_create_gc(UtObject *object, uint32_t drawable) {
  assert(ut_object_is_x11_client(object));

  uint32_t id = ut_x11_client_create_resource_id(object);

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, id);
  ut_x11_buffer_append_card32(request, drawable);
  uint32_t mask = 0x00000000;
  ut_x11_buffer_append_card32(request, mask);
  // FIXME: values

  ut_x11_client_send_request(object, 55, 0, request);

  return id;
}

void ut_x11_client_free_gc(UtObject *object, uint32_t gc) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, gc);

  ut_x11_client_send_request(object, 60, 0, request);
}

void ut_x11_client_clear_area(UtObject *object, uint32_t window, int16_t x,
                              int16_t y, uint16_t width, uint16_t height,
                              bool exposures) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, window);
  ut_x11_buffer_append_int16(request, x);
  ut_x11_buffer_append_int16(request, y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(object, 61, exposures ? 1 : 0, request);
}

void ut_x11_client_copy_area(UtObject *object, uint32_t src_drawable,
                             uint32_t dst_drawable, uint32_t gc, int16_t src_x,
                             int16_t src_y, int16_t dst_x, int16_t dst_y,
                             uint16_t width, uint16_t height) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, src_drawable);
  ut_x11_buffer_append_card32(request, dst_drawable);
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_int16(request, src_x);
  ut_x11_buffer_append_int16(request, src_y);
  ut_x11_buffer_append_int16(request, dst_x);
  ut_x11_buffer_append_int16(request, dst_y);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);

  ut_x11_client_send_request(object, 62, 0, request);
}

void ut_x11_client_put_image(UtObject *object, uint32_t drawable, uint32_t gc,
                             UtX11ImageFormat format, uint16_t width,
                             uint16_t height, uint8_t depth, int16_t dst_x,
                             int16_t dst_y, uint8_t *data, size_t data_length) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, drawable);
  ut_x11_buffer_append_card32(request, gc);
  ut_x11_buffer_append_card16(request, width);
  ut_x11_buffer_append_card16(request, height);
  ut_x11_buffer_append_int16(request, dst_x);
  ut_x11_buffer_append_int16(request, dst_y);
  ut_x11_buffer_append_card8(request, 0); // left_pad);
  ut_x11_buffer_append_card8(request, depth);
  ut_x11_buffer_append_padding(request, 2);
  ut_x11_buffer_append_block(request, data, data_length);
  ut_x11_buffer_append_align_padding(request, 4);

  ut_x11_client_send_request(object, 72, format, request);
}

void ut_x11_client_list_extensions(UtObject *object,
                                   UtX11ListExtensionsCallback callback,
                                   void *user_data, UtObject *cancel) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_client_send_request_with_reply(
      object, 99, 0, request, decode_list_extensions_reply,
      handle_list_extensions_error, callback_data_new(callback, user_data),
      cancel);
}

void ut_x11_client_bell(UtObject *object) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_client_send_request(object, 108, 0, request);
}

void ut_x11_client_kill_client(UtObject *object, uint32_t resource) {
  assert(ut_object_is_x11_client(object));

  UtObjectRef request = ut_x11_buffer_new();
  ut_x11_buffer_append_card32(request, resource);

  ut_x11_client_send_request(object, 113, 0, request);
}

UtObject *ut_x11_client_get_mit_shm_extension(UtObject *object) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;
  return self->mit_shm_extension;
}

UtObject *ut_x11_client_get_present_extension(UtObject *object) {
  assert(ut_object_is_x11_client(object));
  UtX11Client *self = (UtX11Client *)object;
  return self->present_extension;
}

bool ut_object_is_x11_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
