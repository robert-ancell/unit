#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;

  UtObject *input_stream;
  UtObject *callback_object;
  UtX11AuthFileDecodeCallback callback;

  // Decoded records.
  UtObject *records;
} UtX11AuthFileDecoder;

static bool read_uint16(UtObject *data, size_t *offset, uint16_t *value) {
  size_t data_length = ut_list_get_length(data);
  if (*offset + 2 > data_length) {
    return false;
  }

  *value = ut_uint8_list_get_uint16_be(data, *offset);
  *offset += 2;
  return true;
}

static bool read_bytes(UtObject *data, size_t *offset, UtObject **value) {
  uint16_t length;
  if (!read_uint16(data, offset, &length)) {
    return false;
  }

  size_t data_length = ut_list_get_length(data);
  if (*offset + length > data_length) {
    return false;
  }

  *value = ut_list_get_sublist(data, *offset, length);
  *offset += length;
  return true;
}

static bool read_string(UtObject *data, size_t *offset, char **value) {
  UtObjectRef string_data = NULL;
  if (!read_bytes(data, offset, &string_data)) {
    return false;
  }

  UtObjectRef string = ut_string_new_from_ascii(string_data);
  *value = ut_string_take_text(string);
  return true;
}

static UtObject *read_auth_record(UtObject *data, size_t *offset) {
  uint16_t family;
  UtObjectRef address = NULL;
  ut_cstring_ref display = NULL;
  ut_cstring_ref authorization_name = NULL;
  UtObjectRef authorization_data = NULL;
  if (!read_uint16(data, offset, &family) ||
      !read_bytes(data, offset, &address) ||
      !read_string(data, offset, &display) ||
      !read_string(data, offset, &authorization_name) ||
      !read_bytes(data, offset, &authorization_data)) {
    return NULL;
  }

  UtObjectRef address_copy = ut_list_copy(address);
  UtObjectRef authorization_data_copy = ut_list_copy(authorization_data);
  return ut_x11_auth_record_new(family, address_copy, display,
                                authorization_name, authorization_data_copy);
}

static size_t read_cb(UtObject *object, UtObject *data, bool complete) {
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;

  size_t data_length = ut_list_get_length(data);

  size_t offset = 0;
  while (offset < data_length) {
    size_t o = offset;
    UtObjectRef record = read_auth_record(data, &o);
    if (record == NULL) {
      if (complete) {
        // FIXME: Corrupt
      }
      break;
    }

    ut_list_append(self->records, record);
    offset = o;
  }

  if (complete) {
    ut_input_stream_close(self->input_stream);
    if (self->callback_object != NULL && self->callback != NULL) {
      self->callback(self->callback_object);
    }
  }

  return offset;
}

static void ut_x11_auth_file_decoder_init(UtObject *object) {
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;
  self->records = ut_object_list_new();
}

static void ut_x11_auth_file_decoder_cleanup(UtObject *object) {
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;

  ut_input_stream_close(self->input_stream);

  ut_object_unref(self->input_stream);
  ut_object_weak_unref(&self->callback_object);
  ut_object_unref(self->records);
}

static UtObjectInterface object_interface = {
    .type_name = "UtX11AuthFileDecoder",
    .init = ut_x11_auth_file_decoder_init,
    .cleanup = ut_x11_auth_file_decoder_cleanup,
    .interfaces = {{NULL, NULL}}};

UtObject *ut_x11_auth_file_decoder_new(UtObject *input_stream) {
  assert(input_stream != NULL);
  UtObject *object =
      ut_object_new(sizeof(UtX11AuthFileDecoder), &object_interface);
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;
  self->input_stream = ut_object_ref(input_stream);
  return object;
}

void ut_x11_auth_file_decoder_decode(UtObject *object,
                                     UtObject *callback_object,
                                     UtX11AuthFileDecodeCallback callback) {
  assert(ut_object_is_x11_auth_file_decoder(object));
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;
  assert(callback != NULL);
  assert(self->callback == NULL);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  ut_input_stream_read(self->input_stream, object, read_cb);
}

UtObject *ut_x11_auth_file_decoder_get_records(UtObject *object) {
  assert(ut_object_is_x11_auth_file_decoder(object));
  UtX11AuthFileDecoder *self = (UtX11AuthFileDecoder *)object;
  return self->records;
}

bool ut_object_is_x11_auth_file_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
