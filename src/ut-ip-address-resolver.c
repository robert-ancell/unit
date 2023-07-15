#include <assert.h>
#include <netdb.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *name;
  UtObject *callback_object;
  UtIPAddressResolverLookupCallback callback;
} LookupData;

static void lookup_data_cleanup(UtObject *object) {
  LookupData *self = (LookupData *)object;
  free(self->name);
  ut_object_weak_unref(&self->callback_object);
}

static UtObjectInterface lookup_data_object_interface = {
    .type_name = "LookupData", .cleanup = lookup_data_cleanup};

static UtObject *lookup_data_new(const char *name, UtObject *callback_object,
                                 UtIPAddressResolverLookupCallback callback) {
  UtObject *object =
      ut_object_new(sizeof(LookupData), &lookup_data_object_interface);
  LookupData *self = (LookupData *)object;
  self->name = ut_cstring_new(name);
  ut_object_weak_ref(callback_object, &self->callback_object);
  self->callback = callback;
  return object;
}

typedef struct {
  UtObject object;
} UtIPAddressResolver;

static UtObject *lookup_thread_cb(UtObject *data_) {
  LookupData *data = (LookupData *)data_;

  struct addrinfo hints = {.ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP};
  struct addrinfo *addresses;
  getaddrinfo(data->name, NULL, &hints, &addresses);

  UtObject *ip_addresses = ut_list_new();
  for (struct addrinfo *address = addresses; address != NULL;
       address = address->ai_next) {
    if (address->ai_family == AF_INET) {
      struct sockaddr_in *a = (struct sockaddr_in *)address->ai_addr;
      UtObjectRef ip_address = ut_ipv4_address_new(ntohl(a->sin_addr.s_addr));
      ut_list_append(ip_addresses, ip_address);
    } else if (address->ai_family == AF_INET6) {
      struct sockaddr_in6 *a = (struct sockaddr_in6 *)address->ai_addr;
      UtObjectRef ip_address = ut_ipv6_address_new(a->sin6_addr.s6_addr);
      ut_list_append(ip_addresses, ip_address);
    }
  }

  freeaddrinfo(addresses);

  return ip_addresses;
}

static void lookup_result_cb(UtObject *object, UtObject *result) {
  LookupData *data = (LookupData *)object;

  if (data->callback_object != NULL && data->callback != NULL) {
    data->callback(data->callback_object, result);
  }
}

static UtObjectInterface object_interface = {.type_name =
                                                 "UtIPAddressResolver"};

UtObject *ut_ip_address_resolver_new() {
  return ut_object_new(sizeof(UtIPAddressResolver), &object_interface);
}

void ut_ip_address_resolver_lookup(UtObject *object, const char *name,
                                   UtObject *callback_object,
                                   UtIPAddressResolverLookupCallback callback) {
  assert(ut_object_is_ip_address_resolver(object));

  // FIXME: Cancel thread if this UtIPAddressResolver is deleted.
  UtObject *data = lookup_data_new(name, callback_object, callback);
  ut_event_loop_add_worker_thread(lookup_thread_cb, data, data,
                                  lookup_result_cb);
}

bool ut_object_is_ip_address_resolver(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
