#include <assert.h>
#include <netdb.h>
#include <stdlib.h>

#include "ut.h"

typedef struct {
  UtObject object;
  char *name;
  UtIPAddressResolverLookupCallback callback;
  void *user_data;
  UtObject *cancel;
} LookupData;

static void lookup_data_cleanup(UtObject *object) {
  LookupData *self = (LookupData *)object;
  free(self->name);
  ut_object_unref(self->cancel);
}

static UtObjectInterface lookup_data_object_interface = {
    .type_name = "LookupData", .cleanup = lookup_data_cleanup};

static UtObject *lookup_data_new(const char *name,
                                 UtIPAddressResolverLookupCallback callback,
                                 void *user_data, UtObject *cancel) {
  UtObject *object =
      ut_object_new(sizeof(LookupData), &lookup_data_object_interface);
  LookupData *self = (LookupData *)object;
  self->name = ut_cstring_new(name);
  self->callback = callback;
  self->user_data = user_data;
  self->cancel = ut_object_ref(cancel);
  return object;
}

typedef struct {
  UtObject object;
  UtObject *cancel;
} UtIPAddressResolver;

static void *lookup_thread_cb(UtObject *data_) {
  LookupData *data = (LookupData *)data_;

  struct addrinfo hints = {.ai_socktype = SOCK_STREAM,
                           .ai_protocol = IPPROTO_TCP};
  struct addrinfo *addresses;
  getaddrinfo(data->name, NULL, &hints, &addresses);
  return addresses;
}

static void lookup_result_cb(void *user_data, void *result) {
  LookupData *data = user_data;
  struct addrinfo *addresses = result;

  assert(addresses != NULL);
  UtObjectRef ip_addresses = ut_list_new();
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

  if (!ut_cancel_is_active(data->cancel) && data->callback != NULL) {
    data->callback(data->user_data, ip_addresses);
  }

  freeaddrinfo(addresses);
}

static void ut_ip_address_resolver_init(UtObject *object) {
  UtIPAddressResolver *self = (UtIPAddressResolver *)object;
  self->cancel = ut_cancel_new();
}

static void ut_ip_address_resolver_cleanup(UtObject *object) {
  UtIPAddressResolver *self = (UtIPAddressResolver *)object;
  ut_cancel_activate(self->cancel);
  ut_object_unref(self->cancel);
}

static UtObjectInterface object_interface = {
    .type_name = "UtIPAddressResolver",
    .init = ut_ip_address_resolver_init,
    .cleanup = ut_ip_address_resolver_cleanup};

UtObject *ut_ip_address_resolver_new() {
  return ut_object_new(sizeof(UtIPAddressResolver), &object_interface);
}

void ut_ip_address_resolver_lookup(UtObject *object, const char *name,
                                   UtIPAddressResolverLookupCallback callback,
                                   void *user_data, UtObject *cancel) {
  assert(ut_object_is_ip_address_resolver(object));
  UtIPAddressResolver *self = (UtIPAddressResolver *)object;

  // FIXME: Cancel thread if this UtIPAddressResolver is deleted.
  UtObject *data = lookup_data_new(name, callback, user_data, cancel);
  ut_event_loop_add_worker_thread(lookup_thread_cb, data, lookup_result_cb,
                                  data, self->cancel);
}

bool ut_object_is_ip_address_resolver(UtObject *object) {
  return ut_object_is_type(object, &object_interface);
}
