#include <assert.h>

#include "ut-cancel.h"
#include "ut-cstring.h"
#include "ut-dns-client.h"
#include "ut-input-stream.h"
#include "ut-ipv4-address.h"
#include "ut-ipv6-address.h"
#include "ut-list.h"
#include "ut-udp-datagram.h"
#include "ut-udp-socket.h"
#include "ut-uint8-array.h"
#include "ut-uint8-list.h"

#include <stdio.h>

typedef struct {
  UtObject object;
  uint16_t id;
  UtDnsLookupCallback callback;
  void *user_data;
  UtObject *cancel;
} UtDnsQuery;

static void ut_dns_query_cleanup(UtObject *object) {
  UtDnsQuery *self = (UtDnsQuery *)object;
  ut_object_unref(self->cancel);
}

static UtObjectInterface query_object_interface = {
    .type_name = "DnsQuery", .cleanup = ut_dns_query_cleanup};

typedef struct {
  UtObject object;
  UtObject *server_address;
  uint16_t port;
  UtObject *socket;
  uint16_t next_id;
  UtObject *queries;
  UtObject *cancel;
} UtDnsClient;

static UtDnsQuery *get_query(UtDnsClient *self, uint16_t id) {
  size_t queries_length = ut_list_get_length(self->queries);
  for (size_t i = 0; i < queries_length; i++) {
    UtObjectRef query_object = ut_list_get_element(self->queries, i);
    UtDnsQuery *query = (UtDnsQuery *)query_object;
    if (query->id == id) {
      return query;
    }
  }

  return NULL;
}

static uint16_t add_query(UtDnsClient *self, UtDnsLookupCallback callback,
                          void *user_data, UtObject *cancel) {
  UtObjectRef query_object =
      ut_object_new(sizeof(UtDnsQuery), &query_object_interface);
  UtDnsQuery *query = (UtDnsQuery *)query_object;

  while (true) {
    // FIXME: Use a random number
    query->id = self->next_id++;
    if (get_query(self, query->id) == NULL) {
      break;
    }
  }

  query->callback = callback;
  query->user_data = user_data;
  query->cancel = ut_object_ref(cancel);
  ut_list_append(self->queries, query_object);

  return query->id;
}

static void write_name(UtObject *message, const char *name) {
  const char *c = name;
  while (*c != '\0') {
    const char *label_start = c;
    size_t label_length = 0;
    while (*c != '.' && *c != '\0') {
      label_length++;
      c++;
    }
    if (*c == '.') {
      c++;
    }
    assert(label_length <= 63);
    ut_uint8_list_append(message, label_length);
    ut_uint8_list_append_block(message, (uint8_t *)label_start, label_length);
  }

  ut_uint8_list_append(message, 0);
}

static void write_question(UtObject *message, const char *name, uint16_t type,
                           uint16_t class) {
  write_name(message, name);
  ut_uint8_list_append_uint16_be(message, type);
  ut_uint8_list_append_uint16_be(message, class);
}

static void write_header(UtObject *message, uint16_t id, uint16_t flags,
                         uint16_t n_questions, uint16_t n_answers,
                         uint16_t n_authorities,
                         uint16_t n_additional_records) {
  ut_uint8_list_append_uint16_be(message, id);
  ut_uint8_list_append_uint16_be(message, flags);
  ut_uint8_list_append_uint16_be(message, n_questions);
  ut_uint8_list_append_uint16_be(message, n_answers);
  ut_uint8_list_append_uint16_be(message, n_authorities);
  ut_uint8_list_append_uint16_be(message, n_additional_records);
}

static void write_lookup(UtObject *message, uint16_t id, uint16_t flags,
                         const char *name, uint16_t type, uint16_t class) {
  write_header(message, id, flags, 1, 0, 0, 0);
  write_question(message, name, type, class);
}

static void write_ipv4_lookup(UtObject *message, uint16_t id, uint16_t flags,
                              const char *name) {
  write_lookup(message, id, flags, name, 1, 1);
}

static void write_ipv6_lookup(UtObject *message, uint16_t id, uint16_t flags,
                              const char *name) {
  write_lookup(message, id, flags, name, 28, 1);
}

static char *read_name(UtObject *message, size_t *offset) {
  size_t pointer_offset;
  UtObjectRef name = ut_uint8_array_new();
  size_t label_count = 0;
  while (true) {
    uint8_t length = ut_uint8_list_get_element(message, *offset);
    (*offset)++;
    if (length == 0) {
      ut_uint8_list_append(name, '\0');
      return (char *)ut_uint8_list_take_data(name);
    }
    if ((length & 0xc0) == 0) {
      if (label_count > 0) {
        ut_uint8_list_append(name, '.');
      }
      UtObjectRef label = ut_list_get_sublist(message, *offset, length);
      *offset += length;
      ut_list_append_list(name, label);
      label_count++;
    } else if ((length & 0xc0) == 0xc0) {
      uint8_t length2 = ut_uint8_list_get_element(message, *offset);
      (*offset)++;
      pointer_offset = ((length & 0x3f) << 8) | length2;
      // FIXME: Validate
      offset = &pointer_offset;
    } else {
      return NULL;
    }
  }
}

static void read_question(UtObject *message, size_t *offset) {
  ut_cstring_ref name = read_name(message, offset);
  // uint16_t type = ut_uint8_list_get_uint16_be(message, *offset);
  *offset += 2;
  // uint16_t class = ut_uint8_list_get_uint16_be(message, *offset);
  *offset += 2;
}

static void read_resource_record(UtDnsQuery *query, UtObject *message,
                                 size_t *offset) {
  ut_cstring_ref name = read_name(message, offset);
  uint16_t type = ut_uint8_list_get_uint16_be(message, *offset);
  *offset += 2;
  uint16_t class = ut_uint8_list_get_uint16_be(message, *offset);
  *offset += 2;
  // uint32_t ttl = ut_uint8_list_get_uint32_be(message, *offset);
  *offset += 4;
  uint16_t data_length = ut_uint8_list_get_uint16_be(message, *offset);
  *offset += 2;

  UtObjectRef address = NULL;
  switch (type) {
  case 1:
    assert(class == 1);
    assert(data_length == 4);
    address =
        ut_ipv4_address_new(ut_uint8_list_get_uint32_be(message, *offset));
    *offset += data_length;
    break;
  case 28:
    assert(class == 1);
    assert(data_length == 16);
    uint8_t address6[16];
    for (size_t i = 0; i < 16; i++) {
      address6[i] = ut_uint8_list_get_element(message, *offset + i);
    }
    address = ut_ipv6_address_new(address6);
    *offset += data_length;
    break;
  default:
    *offset += data_length;
    break;
  }

  // FIXME: Check correct result
  if (address != NULL && !ut_cancel_is_active(query->cancel)) {
    query->callback(query->user_data, address);
  }
}

static void process_message(UtDnsClient *self, UtObject *message) {
  size_t message_length = ut_list_get_length(message);
  if (message_length < 12) {
    return;
  }
  uint16_t id = ut_uint8_list_get_uint16_be(message, 0);
  // uint16_t flags = ut_uint8_list_get_uint16_be(message, 2);
  uint16_t n_questions = ut_uint8_list_get_uint16_be(message, 4);
  uint16_t n_answers = ut_uint8_list_get_uint16_be(message, 6);
  uint16_t n_authorities = ut_uint8_list_get_uint16_be(message, 8);
  uint16_t n_additional_records = ut_uint8_list_get_uint16_be(message, 10);

  UtDnsQuery *query = get_query(self, id);
  if (query == NULL) {
    return;
  }

  size_t offset = 12;
  for (size_t i = 0; i < n_questions; i++) {
    read_question(message, &offset);
  }
  for (size_t i = 0; i < n_answers; i++) {
    read_resource_record(query, message, &offset);
  }
  for (size_t i = 0; i < n_authorities; i++) {
    read_resource_record(query, message, &offset);
  }
  for (size_t i = 0; i < n_additional_records; i++) {
    read_resource_record(query, message, &offset);
  }
}

static size_t read_cb(void *user_data, UtObject *datagram, bool complete) {
  UtDnsClient *self = user_data;

  // FIXME: Validate address
  process_message(self, ut_udp_datagram_get_data(datagram));

  return 1;
}

static void ut_dns_client_init(UtObject *object) {
  UtDnsClient *self = (UtDnsClient *)object;
  self->queries = ut_list_new();
  self->cancel = ut_cancel_new();
}

static void ut_dns_client_cleanup(UtObject *object) {
  UtDnsClient *self = (UtDnsClient *)object;
  ut_cancel_activate(self->cancel);
  ut_object_unref(self->server_address);
  ut_object_unref(self->socket);
  ut_object_unref(self->queries);
  ut_object_unref(self->cancel);
}

static UtObjectInterface object_interface = {.type_name = "DnsClient",
                                             .init = ut_dns_client_init,
                                             .cleanup = ut_dns_client_cleanup};

UtObject *ut_dns_client_new(UtObject *server_address, uint16_t port) {
  UtObject *object = ut_object_new(sizeof(UtDnsClient), &object_interface);
  UtDnsClient *self = (UtDnsClient *)object;
  self->server_address = ut_object_ref(server_address);
  self->port = port;
  self->socket = ut_udp_socket_new_ipv4();
  ut_input_stream_read(self->socket, read_cb, self, self->cancel);
  return object;
}

void ut_dns_client_lookup_ipv4(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel) {
  assert(ut_object_is_dns_client(object));
  UtDnsClient *self = (UtDnsClient *)object;

  assert(callback != NULL);

  uint16_t id = add_query(self, callback, user_data, cancel);

  UtObjectRef message = ut_uint8_array_new();
  write_ipv4_lookup(message, id, 0, name);
  ut_udp_socket_send(self->socket, self->server_address, self->port, message);
}

void ut_dns_client_lookup_ipv6(UtObject *object, const char *name,
                               UtDnsLookupCallback callback, void *user_data,
                               UtObject *cancel) {
  assert(ut_object_is_dns_client(object));
  UtDnsClient *self = (UtDnsClient *)object;

  assert(callback != NULL);

  uint16_t id = add_query(self, callback, user_data, cancel);

  UtObjectRef message = ut_uint8_array_new();
  write_ipv6_lookup(message, id, 0, name);
  ut_udp_socket_send(self->socket, self->server_address, self->port, message);
}

bool ut_object_is_dns_client(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
