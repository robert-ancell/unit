#include <assert.h>

#include "ut.h"

typedef struct {
  UtObject object;
  UtObject *address;
  uint16_t port;
  UtObject *data;
} UtUdpDatagram;

static void ut_udp_datagram_cleanup(UtObject *object) {
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  ut_object_unref(self->address);
  ut_object_unref(self->data);
}

static char *ut_udp_datagram_to_string(UtObject *object) {
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  ut_cstring_ref address_text = ut_ip_address_to_string(self->address);
  ut_cstring_ref data_text = ut_object_to_string(self->data);
  UtObjectRef string =
      ut_string_new_printf("<UtUdpDatagram>(%s, %d", address_text, self->port);
  size_t data_length = ut_list_get_length(self->data);
  if (data_length > 0) {
    ut_string_append(string, ", ");
    for (size_t i = 0; i < data_length; i++) {
      ut_string_append_printf(string, "%02x",
                              ut_uint8_list_get_element(self->data, i));
    }
  }
  ut_string_append(string, ")");
  return ut_string_take_text(string);
}

static UtObjectInterface object_interface = {.type_name = "UtUdpDatagram",
                                             .cleanup = ut_udp_datagram_cleanup,
                                             .to_string =
                                                 ut_udp_datagram_to_string};

UtObject *ut_udp_datagram_new(UtObject *address, uint16_t port,
                              UtObject *data) {
  UtObject *object = ut_object_new(sizeof(UtUdpDatagram), &object_interface);
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  self->address = ut_object_ref(address);
  self->port = port;
  self->data = ut_object_ref(data);
  return object;
}

UtObject *ut_udp_datagram_get_address(UtObject *object) {
  assert(ut_object_is_udp_datagram(object));
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  return self->address;
}

uint16_t ut_udp_datagram_get_port(UtObject *object) {
  assert(ut_object_is_udp_datagram(object));
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  return self->port;
}

UtObject *ut_udp_datagram_get_data(UtObject *object) {
  assert(ut_object_is_udp_datagram(object));
  UtUdpDatagram *self = (UtUdpDatagram *)object;
  return self->data;
}

bool ut_object_is_udp_datagram(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
