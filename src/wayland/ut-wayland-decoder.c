#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *data;
  size_t offset;
} UtWaylandDecoder;

uint32_t get_uint(UtWaylandDecoder *self) {
  // FIXME: Read at native endian
  uint32_t value = ut_uint8_list_get_uint32_le(self->data, self->offset);
  self->offset += 4;
  return value;
}

static void align(UtWaylandDecoder *self) {
  size_t x = self->offset % 4;
  if (x != 0) {
    self->offset += 4 - x;
  }
}

static void ut_wayland_decoder_cleanup(UtObject *object) {
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;
  ut_object_unref(self->data);
}

static UtObjectInterface object_interface = {
    .type_name = "UtWaylandDecoder", .cleanup = ut_wayland_decoder_cleanup};

UtObject *ut_wayland_decoder_new(UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtWaylandDecoder), &object_interface);
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;
  self->data = ut_object_ref(data);
  return object;
}

UtObject *ut_wayland_decoder_get_data(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;
  return self->data;
}

uint32_t ut_wayland_decoder_get_uint(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;
  return get_uint(self);
}

int32_t ut_wayland_decoder_get_int(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;
  return (int32_t)get_uint(self);
}

double ut_wayland_decoder_get_fixed(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;

  uint32_t value = get_uint(self);
  double sign = (value & 0x80000000) != 0 ? -1 : 1;
  uint32_t integer = (value >> 8) & 0x7ffffff;
  uint32_t fraction = value & 0xff;
  return sign * (integer + fraction / 256.0);
}

char *ut_wayland_decoder_get_string(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;

  uint32_t string_length = get_uint(self);
  UtObjectRef string =
      ut_list_get_sublist(self->data, self->offset, string_length);
  // FIXME: Validate last character is '\0'
  self->offset += string_length;
  align(self);

  return (char *)ut_uint8_list_take_data(string);
}

UtObject *ut_wayland_decoder_get_uint_array(UtObject *object) {
  assert(ut_object_is_wayland_decoder(object));
  UtWaylandDecoder *self = (UtWaylandDecoder *)object;

  uint32_t array_length = get_uint(self);
  assert(array_length % 4 == 0);
  size_t value_length = array_length / 4;
  UtObjectRef value = ut_uint32_list_new();
  for (size_t i = 0; i < value_length; i++) {
    ut_uint32_list_append(value, get_uint(self));
  }
  align(self);

  return ut_object_ref(value);
}

bool ut_object_is_wayland_decoder(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
