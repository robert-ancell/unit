#include <stdio.h>

#include "ut.h"

static void resolve_cb(UtObject *object, UtObject *addresses) {
  ut_cstring_ref text = ut_object_to_string(addresses);
  printf("%s\n", text);
  ut_event_loop_return(NULL);
}

int main(int argc, char **argv) {
  UtObjectRef resolver = ut_ip_address_resolver_new();
  UtObjectRef dummy_object = ut_null_new();
  ut_ip_address_resolver_lookup(resolver, "example.com", dummy_object,
                                resolve_cb);

  ut_event_loop_run();

  return 0;
}
